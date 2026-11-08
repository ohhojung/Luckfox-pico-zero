#include "protocol.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hb_mac(const char *s, char out[13]) {
    size_t i, n;
    unsigned int first = 0;
    if (!s) return -1;
    n = strcspn(s, "\r\n");
    if (s[n] && strspn(s+n, "\r\n") != strlen(s+n)) return -1;
    if (n != 17 && n != 12) return -1;
    for (i=0; i<12; ++i) {
        size_t at = n == 17 ? i+i/2 : i;
        if (!isxdigit((unsigned char)s[at])) return -1;
        out[i] = (char)toupper((unsigned char)s[at]);
    }
    if (n == 17) for (i=2; i<17; i+=3) if(s[i]!=':') return -1;
    out[12]=0;
    if (sscanf(out, "%2x", &first)!=1 || (first&1) ||
        !strcmp(out,"000000000000")) return -1;
    return 0;
}

int hb_payload(char *out, size_t size, const char *mac, int rtsp, int wg) {
    char checked[13];
    int n;
    if (hb_mac(mac,checked)) return -1;
    n=snprintf(out,size,"{\"mac\":\"%s\",\"status\":\"online\","
        "\"rtsp_ok\":%s,\"wg_ok\":%s,\"config_version\":0}",
        checked,rtsp?"true":"false",wg?"true":"false");
    return n<0 || (size_t)n>=size ? -1 : n;
}

static int equal_n(const char *s, size_t n, const char *word) {
    size_t i;
    if (strlen(word)!=n) return 0;
    for(i=0;i<n;i++) if(tolower((unsigned char)s[i])!=word[i]) return 0;
    return 1;
}

int hb_http(const char *raw, size_t len, int eof, int *status,
            char *body, size_t cap) {
    const char *end, *p, *line, *colon, *v, *ve;
    size_t head, have, wanted=0, used=0, pos=0;
    int has_length=0, chunked=0, code;
    /* Caller supplies a NUL terminator; embedded NUL in headers is invalid. */
    end=strstr(raw,"\r\n\r\n");
    if(!end) return eof || len>8192 ? -1 : 0;
    head=(size_t)(end-raw)+4;
    if(head>8192 || head>len || cap==0) return -1;
    line=strstr(raw,"\r\n");
    if(!line || (strncmp(raw,"HTTP/1.0 ",9) && strncmp(raw,"HTTP/1.1 ",9)) ||
       line-raw<12 || !isdigit((unsigned char)raw[9]) ||
       !isdigit((unsigned char)raw[10]) || !isdigit((unsigned char)raw[11]) ||
       (line-raw>12 && raw[12]!=' ')) return -1;
    code=(raw[9]-'0')*100+(raw[10]-'0')*10+raw[11]-'0';
    if(code<200 || code>599) return -1;
    p=line+2;
    while(p<end) {
        line=strstr(p,"\r\n");
        if(!line || line>end) return -1;
        colon=memchr(p,':',(size_t)(line-p));
        if(!colon) return -1;
        v=colon+1; while(v<line && (*v==' ' || *v=='\t')) ++v;
        ve=line; while(ve>v && (ve[-1]==' ' || ve[-1]=='\t')) --ve;
        if(equal_n(p,(size_t)(colon-p),"content-length")) {
            size_t n=0;
            if(has_length || v==ve) return -1;
            while(v<ve) {
                if(!isdigit((unsigned char)*v) || n>(cap-1)/10) return -1;
                n=n*10+(size_t)(*v++-'0'); if(n>=cap) return -1;
            }
            wanted=n; has_length=1;
        } else if(equal_n(p,(size_t)(colon-p),"transfer-encoding")) {
            if(chunked || !equal_n(v,(size_t)(ve-v),"chunked")) return -1;
            chunked=1;
        }
        p=line+2;
    }
    if(chunked && has_length) return -1;
    p=raw+head; have=len-head;
    if(chunked) {
        for(;;) {
            size_t n=0, digits=0;
            const char *cr;
            if(pos>=have) return eof?-1:0;
            cr=strstr(p+pos,"\r\n");
            if(!cr) return eof?-1:0;
            v=p+pos;
            while(v<cr && isxdigit((unsigned char)*v)) {
                unsigned int digit=isdigit((unsigned char)*v)?(unsigned)(*v-'0'):
                    (unsigned)(tolower((unsigned char)*v)-'a'+10);
                if(n>(cap-1)/16) return -1;
                n=n*16+digit; ++v; ++digits;
                if(n>=cap) return -1;
            }
            if(!digits || (v<cr && *v!=';')) return -1;
            pos=(size_t)(cr-p)+2;
            if(n==0) {
                if(have-pos<2) return eof?-1:0;
                if(memcmp(p+pos,"\r\n",2) && !strstr(p+pos,"\r\n\r\n"))
                    return eof?-1:0;
                break;
            }
            if(n>cap-1-used) return -1;
            if(have-pos<n+2) return eof?-1:0;
            memcpy(body+used,p+pos,n); used+=n; pos+=n;
            if(memcmp(p+pos,"\r\n",2)) return -1;
            pos+=2;
        }
    } else {
        if(has_length) {
            if(have<wanted) return eof?-1:0;
            used=wanted;
        } else {
            if(!eof) return 0;
            used=have;
        }
        if(used>=cap) return -1;
        memcpy(body,p,used);
    }
    if(memchr(body,0,used)) return -1;
    body[used]=0; *status=code; return 1;
}

/* A bounded JSON recognizer; only an unescaped top-level config_version is used. */
static void ws(const char **p) { while(**p && strchr(" \t\r\n",**p)) ++*p; }
static int string_end(const char **p) {
    if(*(*p)++!='"') return -1;
    while(**p && **p!='"') {
        unsigned char c=(unsigned char)*(*p)++;
        if(c<32) return -1;
        if(c=='\\') {
            int i;
            c=(unsigned char)*(*p)++;
            if(!c) return -1;
            if(c=='u') {
                for(i=0;i<4;i++) { if(!isxdigit((unsigned char)**p)) return -1; ++*p; }
            } else if(!strchr("\"\\/bfnrt",c)) return -1;
        }
    }
    if(**p!='"') return -1;
    ++*p; return 0;
}
static int value(const char **p, int depth) {
    char close;
    if(depth>16) return -1;
    ws(p);
    if(**p=='"') return string_end(p);
    if(**p=='{' || **p=='[') {
        int object=**p=='{'; close=object?'}':']'; ++*p; ws(p);
        if(**p==close) { ++*p; return 0; }
        for(;;) {
            if(object) {
                if(**p!='"' || string_end(p)) return -1;
                ws(p); if(**p!=':') return -1; ++*p;
            }
            if(value(p,depth+1)) return -1;
            ws(p); if(**p==close) { ++*p; return 0; }
            if(**p!=',') return -1;
            ++*p; ws(p);
        }
    }
    if(!strncmp(*p,"true",4) || !strncmp(*p,"null",4)) { *p+=4; return 0; }
    if(!strncmp(*p,"false",5)) { *p+=5; return 0; }
    if(**p=='-') ++*p;
    if(!isdigit((unsigned char)**p)) return -1;
    if(**p=='0') ++*p;
    else while(isdigit((unsigned char)**p)) ++*p;
    if(**p=='.') { ++*p; if(!isdigit((unsigned char)**p)) return -1;
        while(isdigit((unsigned char)**p)) ++*p; }
    if(**p=='e' || **p=='E') { ++*p; if(**p=='+' || **p=='-') ++*p;
        if(!isdigit((unsigned char)**p)) return -1;
        while(isdigit((unsigned char)**p)) ++*p; }
    return 0;
}
int hb_config_version(const char *json, unsigned long *version) {
    const char *p=json, *key, *start, *finish;
    int found=0;
    unsigned long result=0;
    ws(&p); if(*p!='{') return -1; ++p; ws(&p);
    if(*p=='}') return -1;
    for(;;) {
        size_t keylen;
        if(*p!='"') return -1;
        key=p; if(string_end(&p)) return -1; keylen=(size_t)(p-key);
        ws(&p); if(*p!=':') return -1; ++p; ws(&p); start=p;
        if(value(&p,0)) return -1;
        finish=p;
        if(keylen==16 && !memcmp(key,"\"config_version\"",16)) {
            if(found++ || start==finish) return -1;
            while(start<finish) {
                unsigned int digit;
                if(!isdigit((unsigned char)*start)) return -1;
                digit=(unsigned int)(*start++-'0');
                if(result>(ULONG_MAX-digit)/10) return -1;
                result=result*10+digit;
            }
        }
        ws(&p); if(*p=='}') { ++p; break; }
        if(*p!=',') return -1;
        ++p; ws(&p);
    }
    ws(&p); if(*p || found!=1) return -1;
    *version=result; return 0;
}

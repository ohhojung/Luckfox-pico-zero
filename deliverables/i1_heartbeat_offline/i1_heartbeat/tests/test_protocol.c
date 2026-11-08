#include "../src/protocol.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void response(const char *wire, int eof, int expected, int status, const char *body) {
    char output[8192]; int code=0;
    int rc=hb_http(wire,strlen(wire),eof,&code,output,sizeof output);
    assert(rc==expected);
    if(rc==1) { assert(code==status); assert(!strcmp(output,body)); }
}
int main(void) {
    char mac[13],payload[256],wire[1024],output[8192];
    const char *json="{\"config_version\":7}";
    unsigned long v=99; size_t i,n; int code;
    assert(!hb_mac("02:ab:cd:12:ef:34\n",mac));
    assert(!strcmp(mac,"02ABCD12EF34"));
    assert(!hb_mac("02:AB:cD:12:EF:34",mac));
    assert(!strcmp(mac,"02ABCD12EF34"));
    assert(!hb_mac("02abcd12ef34",mac));
    assert(hb_mac("00:00:00:00:00:00",mac)<0);
    assert(hb_mac("ff:ff:ff:ff:ff:ff",mac)<0);
    assert(hb_mac("01:00:00:00:00:01",mac)<0);
    assert(hb_mac("02-ab-cd-12-ef-34",mac)<0);
    assert(hb_mac("02:ab:cd:12:ef:3z",mac)<0);
    assert(hb_mac("02:ab:cd:12:ef:34\nextra",mac)<0);
    assert(hb_payload(payload,sizeof payload,"02ABCD12EF34",1,0)>0);
    assert(!strcmp(payload,"{\"mac\":\"02ABCD12EF34\",\"status\":\"online\",\"rtsp_ok\":true,\"wg_ok\":false,\"config_version\":0}"));
    assert(!hb_mac("74:24:ca:50:04:af",mac));
    assert(!strcmp(mac,"7424CA5004AF"));
    assert(hb_payload(payload,8,"02ABCD12EF34",1,1)<0);
    assert(!hb_config_version(json,&v) && v==7);
    assert(!hb_config_version(" {\"unused\":[true,null,{\"x\":1.5}],\"config_version\":0} ",&v) && v==0);
    assert(!hb_config_version("{\"config_version\":12,\"message\":\"a\\\"b\"}",&v) && v==12);
    assert(hb_config_version("{\"nested\":{\"config_version\":4}}",&v)<0);
    assert(hb_config_version("{\"config_version\":\"4\"}",&v)<0);
    assert(hb_config_version("{\"config_version\":-1}",&v)<0);
    assert(hb_config_version("{\"config_version\":1.0}",&v)<0);
    assert(hb_config_version("{\"config_version\":01}",&v)<0);
    assert(hb_config_version("{\"config_version\":2,\"config_version\":3}",&v)<0);
    assert(hb_config_version("{\"config_version\":999999999999999999999999999999}",&v)<0);
    assert(hb_config_version("{\"config_version\":2}trailing",&v)<0);
    assert(hb_config_version("{\"config_version\":2,}",&v)<0);
    assert(hb_config_version("{\"config_version\":2,\"x\":tru}",&v)<0);
    snprintf(wire,sizeof wire,"HTTP/1.1 200 OK\r\nContent-Length: %lu\r\nConnection: keep-alive\r\n\r\n%s",(unsigned long)strlen(json),json);
    response(wire,0,1,200,json); /* Completion must not wait for keep-alive EOF. */
    n=strlen(wire);
    for(i=0;i<n;i++) {
        char prefix[1024]; memcpy(prefix,wire,i); prefix[i]=0;
        assert(hb_http(prefix,i,0,&code,output,sizeof output)==0);
        assert(hb_http(prefix,i,1,&code,output,sizeof output)==-1);
    }
    response("HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n",0,1,403,"");
    response("HTTP/1.0 200 OK\r\n\r\n{}",0,0,0,"");
    response("HTTP/1.0 200 OK\r\n\r\n{}",1,1,200,"{}");
    response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n2\r\n{}\r\n0\r\n\r\n",0,1,200,"{}");
    response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n1;x=y\r\n{\r\n1\r\n}\r\n0\r\nX-Foo: ok\r\n\r\n",0,1,200,"{}");
    response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n2\r\n{",0,0,0,"");
    response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n2\r\n{",1,-1,0,"");
    response("HTTP/1.1 200 OK\r\nContent-Length: 99999999\r\n\r\n",0,-1,0,"");
    response("HTTP/1.1 200 OK\r\nContent-Length: 2\r\nContent-Length: 2\r\n\r\n{}",0,-1,0,"");
    response("HTTP/1.1 200 OK\r\nContent-Length: 2\r\nTransfer-Encoding: chunked\r\n\r\n",0,-1,0,"");
    response("HTTP/1.1 200x\r\n\r\n",1,-1,0,"");
    response("not HTTP\r\n\r\n",1,-1,0,"");
    puts("PASS: MAC validation, JSON payload/types, config parsing, HTTP framing, truncation and size limits; no sockets used.");
    return 0;
}

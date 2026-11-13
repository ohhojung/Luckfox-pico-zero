#define _POSIX_C_SOURCE 200809L
#include "transport.h"
#include <net/if.h>
#include <stdio.h>
#include <string.h>

struct response_buffer {
    char *data;
    size_t used, capacity;
    const volatile sig_atomic_t *cancelled;
};
static size_t receive_body(char *data, size_t size, size_t count, void *userdata) {
    struct response_buffer *buffer=userdata;
    size_t bytes;
    if(size && count>(size_t)-1/size) return 0;
    bytes=size*count;
    if(*buffer->cancelled || bytes>buffer->capacity-1-buffer->used) return 0;
    memcpy(buffer->data+buffer->used,data,bytes);
    buffer->used+=bytes;
    buffer->data[buffer->used]=0;
    return bytes;
}
int post(const char *host, int port, const char *iface, const char *payload, int timeout_ms,
                int *status, char *body, size_t capacity, char *error, const volatile sig_atomic_t *cancelled) {
    CURL *curl=curl_easy_init();
    struct curl_slist *headers=NULL;
    struct response_buffer buffer={body,0,capacity,cancelled};
    char url[128], bound_interface[IF_NAMESIZE+4];
    CURLcode rc=CURLE_FAILED_INIT;
    long http_status=0;
    error[0]=0;
    if(!curl) { snprintf(error,CURL_ERROR_SIZE,"curl_easy_init failed"); return -1; }
    body[0]=0;
    snprintf(url,sizeof url,"http://%s:%d/v1/devices/heartbeat",host,port);
    headers=curl_slist_append(NULL,"Content-Type: application/json");
    if(!headers) { rc=CURLE_OUT_OF_MEMORY; goto done; }
#define SETOPT(option, value) do { rc=curl_easy_setopt(curl,option,value); if(rc!=CURLE_OK) goto done; } while(0)
    SETOPT(CURLOPT_ERRORBUFFER,error);
    SETOPT(CURLOPT_URL,url);
    snprintf(bound_interface,sizeof bound_interface,"if!%s",iface);
    SETOPT(CURLOPT_INTERFACE,bound_interface); /* No fallback to Wi-Fi when the VPN is absent. */
    SETOPT(CURLOPT_PROXY,""); /* VPN target must not use environment proxies. */
    SETOPT(CURLOPT_POST,1L);
    SETOPT(CURLOPT_HTTPHEADER,headers);
    SETOPT(CURLOPT_POSTFIELDS,payload);
    SETOPT(CURLOPT_POSTFIELDSIZE,(long)strlen(payload));
    SETOPT(CURLOPT_TIMEOUT_MS,(long)timeout_ms);
    SETOPT(CURLOPT_CONNECTTIMEOUT_MS,(long)timeout_ms);
    SETOPT(CURLOPT_NOSIGNAL,1L);
    SETOPT(CURLOPT_FOLLOWLOCATION,0L);
    SETOPT(CURLOPT_FAILONERROR,0L); /* Preserve HTTP 403 for application logging. */
    SETOPT(CURLOPT_WRITEFUNCTION,receive_body);
    SETOPT(CURLOPT_WRITEDATA,&buffer);
#undef SETOPT
    rc=curl_easy_perform(curl);
    if(rc==CURLE_OK) rc=curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&http_status);
    if(rc==CURLE_OK) *status=(int)http_status;
done:
    if(rc!=CURLE_OK && !error[0]) snprintf(error,CURL_ERROR_SIZE,"%s",curl_easy_strerror(rc));
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return rc==CURLE_OK?0:-1;
}

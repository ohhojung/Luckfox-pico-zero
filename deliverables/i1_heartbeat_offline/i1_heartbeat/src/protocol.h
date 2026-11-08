#ifndef HEARTBEAT_PROTOCOL_H
#define HEARTBEAT_PROTOCOL_H
#include <stddef.h>
int hb_mac(const char *input, char out[13]);
int hb_payload(char *out, size_t size, const char *mac, int rtsp, int wg);
/* 1 = complete, 0 = need more bytes, -1 = invalid/oversized. */
int hb_http(const char *raw, size_t len, int eof, int *status,
            char *body, size_t capacity);
/* Extract a top-level nonnegative integer. Missing/invalid returns -1. */
int hb_config_version(const char *json, unsigned long *version);
#endif

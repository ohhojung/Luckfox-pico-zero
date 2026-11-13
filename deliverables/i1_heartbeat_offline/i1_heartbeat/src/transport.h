#ifndef HEARTBEAT_TRANSPORT_H
#define HEARTBEAT_TRANSPORT_H
#include <stddef.h>
#include <signal.h>
#include <curl/curl.h>
/* Caller initializes libcurl and supplies CURL_ERROR_SIZE bytes for error. */
int post(const char *host, int port, const char *iface, const char *payload, int timeout_ms,
         int *status, char *body, size_t capacity, char *error,
         const volatile sig_atomic_t *cancelled);
#endif

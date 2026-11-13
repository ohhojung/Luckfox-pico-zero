#ifndef HEARTBEAT_RUNTIME_H
#define HEARTBEAT_RUNTIME_H
#include <signal.h>
extern volatile sig_atomic_t stopping;
extern int use_syslog;
void stop(int sig);
void report(const char *fmt, ...);
long long now_ms(void);
int lock_pidfile(const char *path);
#endif

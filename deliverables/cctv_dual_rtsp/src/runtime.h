#ifndef CCTV_RUNTIME_H
#define CCTV_RUNTIME_H
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
extern volatile sig_atomic_t stopping;
void stop(int signal_number);
void deadline(int sig);
uint64_t monotonic_ms(void);
void limit_log(void);
bool check(const char *operation, int channel, int result);
#endif

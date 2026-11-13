#include "runtime.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
volatile sig_atomic_t stopping;

void stop(int signal_number)
{
    (void)signal_number;
    if (!stopping) alarm(15);
    stopping = 1;
}

uint64_t monotonic_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / 1000000;
}


void deadline(int sig) { (void)sig; _exit(124); }
void limit_log(void)
{
    struct stat st;
    if (fstat(STDOUT_FILENO, &st) == 0 && S_ISREG(st.st_mode) && st.st_size > 1024*1024) {
        fflush(stdout); fflush(stderr);
        if (ftruncate(STDOUT_FILENO, 0) == 0) lseek(STDOUT_FILENO, 0, SEEK_SET);
    }
}

bool check(const char *operation, int channel, int result)
{
    if (result == 0)
        return true;
    fprintf(stderr, "%s channel=%d failed (0x%08x)\n",
            operation, channel, (unsigned)result);
    return false;
}

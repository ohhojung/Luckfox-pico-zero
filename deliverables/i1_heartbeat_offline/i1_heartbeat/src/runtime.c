#define _POSIX_C_SOURCE 200809L
#include "runtime.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

volatile sig_atomic_t stopping;
int use_syslog;
void stop(int sig) { (void)sig; stopping=1; }
void report(const char *fmt, ...) {
    char message[768]; va_list ap;
    va_start(ap,fmt); vsnprintf(message,sizeof message,fmt,ap); va_end(ap);
    if(use_syslog) syslog(LOG_INFO,"%s",message);
    else { puts(message); fflush(stdout); }
}
long long now_ms(void) {
    struct timespec t;
    if(clock_gettime(CLOCK_MONOTONIC,&t)) { perror("clock_gettime"); exit(2); }
    return (long long)t.tv_sec*1000+t.tv_nsec/1000000;
}
int lock_pidfile(const char *path) {
    struct flock lock; char pid[32]; int n;
    int fd=open(path,O_CREAT|O_RDWR,0644);
    if(fd<0) return -1;
    memset(&lock,0,sizeof lock); lock.l_type=F_WRLCK; lock.l_whence=SEEK_SET;
    if(fcntl(fd,F_SETLK,&lock)<0) { close(fd); return -1; }
    n=snprintf(pid,sizeof pid,"%ld\n",(long)getpid());
    if(ftruncate(fd,0) || write(fd,pid,(size_t)n)!=n) { close(fd); return -1; }
    return fd;
}

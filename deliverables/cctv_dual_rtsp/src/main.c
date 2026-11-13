#include "runtime.h"
#include "pipeline.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "-h")) {
        puts("Usage: cctv-dual-rtsp [-c h264|h265] {[-a IQ_DIR] [-F 30 [-T]] | -N}");
        return EXIT_SUCCESS;
    }
    /* File lock survives pipeline retries and prevents multiple camera owners. */
    int lockfd = open("/var/run/cctv-dual-rtsp.lock", O_CREAT|O_RDWR, 0644);
    struct flock lock = { .l_type = F_WRLCK, .l_whence = SEEK_SET };
    if (lockfd < 0 || fcntl(lockfd, F_SETLK, &lock) < 0) {
        fprintf(stderr, "Cannot acquire camera process lock\n");
        if (lockfd >= 0) close(lockfd);
        return EXIT_FAILURE;
    }
    signal(SIGALRM, deadline);
    signal(SIGPIPE, SIG_IGN);
    unsigned restart_attempt;
    int result = EXIT_FAILURE;
    for (restart_attempt = 1; restart_attempt <= 3 && !stopping; ++restart_attempt) {
        optind = 0;
        result = run_once(argc, argv, restart_attempt);
        alarm(0);
        if (result == EXIT_SUCCESS || stopping) break;
        if (restart_attempt < 3) {
            fprintf(stderr, "RECOVERY attempt=%u; restarting pipeline after 3 seconds\n", restart_attempt);
            for (int i=0; i<3 && !stopping; ++i) sleep(1);
        }
    }
    close(lockfd);
    return stopping ? EXIT_SUCCESS : result;
}

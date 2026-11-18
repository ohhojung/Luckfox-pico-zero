#include "runtime.h"
#include "pipeline.h"
#include "config.h"
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* rkipc does not share our advisory lock. Check before touching AIQ/SYS. */
static int rkipc_active(void)
{
    DIR *proc = opendir("/proc");
    if (!proc) { perror("Cannot inspect camera owners"); return 1; }
    struct dirent *entry;
    int busy = 0;
    while ((entry = readdir(proc))) {
        if (entry->d_name[0] < '0' || entry->d_name[0] > '9') continue;
        char path[300], name[64];
        snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);
        FILE *f = fopen(path, "r");
        if (!f) continue;
        if (fgets(name, sizeof(name), f) && !strcmp(name, "rkipc\n")) busy = 1;
        fclose(f);
        if (busy) break;
    }
    closedir(proc);
    return busy;
}

int main(int argc, char **argv)
{
    struct stream_options options;
    optind = 0;
    int parsed = parse_options(argc, argv, &options);
    if (parsed) return parsed > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
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
        if (rkipc_active()) {
            fprintf(stderr, "Camera busy: rkipc is running. Stop it before starting cctv. No AIQ initialization attempted.\n");
            break;
        }
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

#include "status.h"
#include "runtime.h"
#include <stdio.h>
#include <unistd.h>
/* Same atomic status publication principle as BoardServer/watch.c.
 * monotonic_ms and pid must be checked by consumers; stale files are not health. */
void publish_status(const char *state, struct video_stream *v, unsigned attempt)
{
    const char *tmp = "/tmp/cctv-dual-rtsp.status.tmp";
    FILE *f = fopen(tmp, "w");
    if (!f) return;
    fprintf(f, "state=%s\npid=%ld\nmonotonic_ms=%llu\nattempt=%u\n"
               "main_frames=%llu\nsub_frames=%llu\nmain_last_ms=%llu\nsub_last_ms=%llu\n",
            state, (long)getpid(), (unsigned long long)monotonic_ms(), attempt,
            (unsigned long long)v[0].total_frames, (unsigned long long)v[1].total_frames,
            (unsigned long long)v[0].last_frame_ms, (unsigned long long)v[1].last_frame_ms);
    if (fclose(f) == 0) rename(tmp, "/tmp/cctv-dual-rtsp.status");
}

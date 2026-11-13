/* Optional diagnostic: save the same bytes supplied to RTSP, before transport.
 * Bounded to 200 frames / 16 MiB per channel, once per process. */
#include "capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
struct capture { FILE *file; int started; unsigned frames; size_t bytes; uint64_t last_pts; };
static struct capture captures[2];
void capture_close(int channel)
{
    if (channel < 0 || channel >= 2) return;
    struct capture *c = &captures[channel];
    if (!c->file) return;
    int result = fclose(c->file);
    c->file = NULL;
    fprintf(stderr, "CAPTURE channel=%d closed frames=%u bytes=%zu close_result=%d\n",
            channel, c->frames, c->bytes, result);
}
void capture_frame(int channel, const uint8_t *data, size_t size, uint64_t pts, int h264)
{
    if (channel < 0 || channel >= 2 || !data || !size) return;
    struct capture *c = &captures[channel];
    if (!c->started) {
        c->started = 1;
        const char *dir = getenv("CCTV_DUMP_DIR");
        if (!dir || !*dir) return;
        char path[1024];
        int n = snprintf(path, sizeof(path), "%s/live-%d.%s", dir, channel, h264 ? "h264" : "h265");
        if (n < 0 || (size_t)n >= sizeof(path)) return;
        int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd < 0) { perror("CAPTURE open (use an empty directory)"); return; }
        c->file = fdopen(fd, "wb");
        if (!c->file) { close(fd); perror("CAPTURE fdopen"); return; }
        fprintf(stderr, "CAPTURE channel=%d file=%s limit_frames=200 limit_bytes=16777216\n", channel, path);
    }
    if (!c->file) return;
    if (size > 16777216U - c->bytes) { capture_close(channel); return; }
    if (fwrite(data, 1, size, c->file) != size) {
        perror("CAPTURE write"); capture_close(channel); return;
    }
    if (c->frames && pts <= c->last_pts)
        fprintf(stderr, "CAPTURE channel=%d non_increasing_pts previous=%" PRIu64 " current=%" PRIu64 "\n", channel, c->last_pts, pts);
    c->last_pts = pts;
    ++c->frames;
    c->bytes += size;
    if (c->frames >= 200) capture_close(channel);
}

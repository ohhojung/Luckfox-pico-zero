#include "../src/osd.c"
#include <assert.h>
int main(void) {
    for (int scale=2; scale<=4; scale+=2) {
        struct osd_chn o={0}; o.scale=scale;
        o.width=ALIGN16(TEXT_LEN*CELL_W*scale+2*scale);
        o.height=ALIGN16(GLYPH_H*scale+2*scale);
        size_t n=(size_t)o.width*o.height;
        o.mask=calloc(n,1); o.pixels=calloc(n,sizeof(uint32_t)); assert(o.mask && o.pixels);
        render(&o,"2026-11-15 23:59:58");
        size_t text=0,outline=0,clear=0;
        for(size_t i=0;i<n;i++) {text+=o.pixels[i]==COLOR_TEXT;outline+=o.pixels[i]==COLOR_OUTLINE;clear+=o.pixels[i]==COLOR_NONE;}
        assert(text && outline && clear && text+outline+clear==n);
        uint32_t *before=malloc(n*sizeof(uint32_t)); assert(before);
        memcpy(before,o.pixels,n*sizeof(uint32_t));
        render(&o,"2026-11-15 23:59:59");
        assert(memcmp(before,o.pixels,n*sizeof(uint32_t))!=0);
        free(before);free(o.mask);free(o.pixels);
    }
    puts("PASS OSD: both scales, text/outline/transparency, seconds change");
    return 0;
}
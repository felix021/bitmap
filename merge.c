#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    int i, j, g, ret;
    if (argc < 5) {
        printf("usage: %s <r> <g> <b> <dest>\n", argv[0]);
        return 1;
    }
    bmp_t *b[4];
    for (i = 1; i <= 3; i++) {
        b[i] = bmp_init();
        assert(b[i] != NULL);
        ret = bmp_read(b[i], argv[i]);
        if (ret) {
            printf("%d:%s\n", ret, bmp_error(ret));
            return ret;
        }
    }
    for (i = 2; i <= 3; i++) {
        if (b[i]->info.height != b[1]->info.height) {
            printf("height not match");
            return 1;
        }
        if (b[i]->info.width != b[1]->info.width) {
            printf("width not match");
            return 1;
        }
    }

    b[0] = bmp_init();
    assert(b[0] != NULL);
    ret = bmp_create(b[0], argv[4], b[1]->info.width, b[1]->info.height, BMP_BIT24);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }

    for (i = 0; i < b[0]->info.height; i++) {
        for (j = 0; j < b[0]->info.width; j++) {
            uint8_t cr, cg, cb;
            cr = bmp_get_byte(b[1], i, j);
            cg = bmp_get_byte(b[2], i, j);
            cb = bmp_get_byte(b[3], i, j);
            *bmp_get_rgb(b[0], i, j) = bmp_make_rgb(cr, cg, cb);
        }
    }

    ret = bmp_write(b[0]);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }


    for (i = 0; i < 4; i++)
        bmp_close(b[i]);

    printf("OK\n");
    return 0;
}

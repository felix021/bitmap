#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    int i, j, g, ret;
    bmp_rgb *c;
    bmp_t *b1 = bmp_init();
    assert(b1 != NULL);

    if (argc < 5) {
        printf("Usage: %s <filename> <red> <green> " 
                "<blue> [Width] [Height]\n", argv[0]);
        return 0;
    }
    int r, g, b;
    r = atoi(argv[2]);
    g = atoi(argv[3]);
    b = atoi(argv[4]);

    int width = 1920, height = 1200;
    if (argc >= 7) {
        width  = atoi(argv[5]);
        height = atoi(argv[6]);
    }

    ret = bmp_create(b1, argv[1], width, height, BMP_BIT1);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }

    *bmp_get_pale_rgb(b1, 0) = bmp_make_rgb(r, g, b);

    for (i = 0; i < b1->info.height; i++) {
        for (j = 0; j < b1->info.width; j++) {
            bmp_set_bit(b1, i, j, 0);
        }
    }

    ret = bmp_write(b1);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }


    bmp_close(b1);

    printf("OK\n");
    return 0;
}

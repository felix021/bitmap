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

    int width = 997, height = 998;

    ret = bmp_create(b1, argv[1], width, height, BMP_BIT4);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }

    *bmp_get_pale_rgb(b1, 0) = bmp_make_rgb(255, 0, 0);
    *bmp_get_pale_rgb(b1, 1) = bmp_make_rgb(0, 255, 0);
    *bmp_get_pale_rgb(b1, 2) = bmp_make_rgb(0, 0, 255);

    int color = 0;
    for (i = 0; i < b1->info.height; i++) {
        for (j = 0; j < b1->info.width; j++) {
            bmp_set_bit4(b1, i, j, color);
            color = (color + 1) % 3;
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

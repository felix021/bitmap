#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    int i, j, g, ret;
    bmp_rgb *c;
    if (argc < 4) {
        printf("usage: %s <from> <to> <color:r|g|b>\n", argv[0]);
        return 1;
    }
    bmp_t *b1 = bmp_init();
    bmp_t *b2 = bmp_init();
    assert(b1 != NULL && b2 != NULL);

    ret = bmp_read(b1, argv[1]);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }
    ret = bmp_create(b2, argv[2], b1->info.width, b1->info.height, BMP_BIT8);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }

    bmp_head_info_dump(b1);
    bmp_head_info_dump(b2);

    for (i = 0; i < 256; i++) {
        switch (argv[3][0]) {
            case 'r':
                *bmp_get_pale_rgb(b2, i) = bmp_make_rgb(i, 0, 0);
                break;
            case 'g':
                *bmp_get_pale_rgb(b2, i) = bmp_make_rgb(0, i, 0);
                break;
            case 'b':
                *bmp_get_pale_rgb(b2, i) = bmp_make_rgb(0, 0, i);
                break;
        }
    }

    for (i = 0; i < b1->info.height; i++) {
        for (j = 0; j < b1->info.width; j++) {
            switch(b1->info.bits_per_pixel) {
                case BMP_BIT1:
                    c = bmp_get_pale_rgb(b1, bmp_get_bit(b1, i, j));
                    break;
                case BMP_BIT4:
                    c = bmp_get_pale_rgb(b1, bmp_get_bit4(b1, i, j));
                    break;
                case BMP_BIT8:
                    c = bmp_get_pale_rgb(b1, bmp_get_byte(b1, i, j));
                    break;
                case BMP_BIT24:
                case BMP_BIT32:
                    c = bmp_get_rgb(b1, i, j);
                    break;
                default:
                    printf("[Not supported yet]\n");
                    g = 0;
                    break;
            }
            switch (argv[3][0]) {
                case 'r':
                    bmp_set_byte(b2, i, j, c->red);
                    break;
                case 'g':
                    bmp_set_byte(b2, i, j, c->green);
                    break;
                case 'b':
                    bmp_set_byte(b2, i, j, c->blue);
                    break;
            }
        }
    }

    ret = bmp_write(b2);
    if (ret) {
        printf("%d:%s\n", ret, bmp_error(ret));
        return ret;
    }

    bmp_close(b1);
    bmp_close(b2);

    printf("OK\n");
    return 0;
}

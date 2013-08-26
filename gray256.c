#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"

int main()
{
    bmp_t *b = bmp_init();
    assert(b != NULL);

    bmp_create(b, "gray256.bmp", 16, 16, BMP_BIT8);

    int i, j;
    for (i = 0; i < (1 << 8); i++) {
        *bmp_get_pale_rgb(b, i) = bmp_make_rgb(i, i, i);
    }

    for (i = 0; i < (1 << (8 / 2)); i++) {
        for (j = 0; j < (1 << (8 / 2)); j++) {
            bmp_set_byte(b, i, j, 255 - (i * (1 << (8 / 2)) + j));
        }
    }

    bmp_head_info_dump(b);
    bmp_pale_dump(b);

    bmp_write(b);

    bmp_close(b);
    return 0;
}

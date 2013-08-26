#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"

int main()
{
    bmp_t *b = bmp_init();
    assert(b != NULL);

    bmp_create(b, "gray16.bmp", 4, 4, BMP_BIT4);

    int i, j;
    for (i = 0; i < 16; i++) {
        j = i * 16;
        *bmp_get_pale_rgb(b, i) = bmp_make_rgb(j, j, j);
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            bmp_set_bit4(b, i, j, i * 4 + j);
        }
    }

    bmp_dump(b);

    bmp_write(b);

    bmp_close(b);
    return 0;
}

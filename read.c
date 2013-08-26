#include <stdio.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int ret;

    bmp_t *b = bmp_init();
    if (NULL == b) {
        printf("can't init bmp_t\n");
        return 1;
    }

    ret = bmp_read(b, argv[1]);
    if (ret) {
        printf("ret = %d, %s\n", ret, bmp_error(ret));
        return 2;
    }

    bmp_dump(b);

    bmp_close(b);
    return 0;
}

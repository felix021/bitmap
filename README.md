bitmap
======

a simple bitmap library without any dependency.

两年前写的一个bitmap库……放到github上供着吧:)

关键文件是 `bitmap.h` 和 `bitmap.c` 具体使用可以参考其他 c 源码，如`read.c`：


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

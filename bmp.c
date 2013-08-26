#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bmp.h"

const bmp_rgb BMP_RGB_WHITE = {0xff, 0xff, 0xff};
const bmp_rgb BMP_RGB_BLACK = {0, 0, 0};
const bmp_rgb BMP_RGB_RED   = {0, 0, 0xff};
const bmp_rgb BMP_RGB_GREEN = {0, 0xff, 0};
const bmp_rgb BMP_RGB_BLUE  = {0xff, 0, 0};

static const uint16_t bits_supported[] = {
    BMP_BIT1, BMP_BIT4, BMP_BIT8, BMP_BIT24, BMP_BIT32,
};
#define n_bits_supported (sizeof(bits_supported) / sizeof(*bits_supported))

static const char * _bmp_error_str[] = {
    "OK",                      //0
    "fopen failed",            //1
    "fseek failed",            //2
    "fread failed",            //3
    "filetype != BM",          //4
    "infosize != 0x28",        //5
    "malloc failed",           //6
    "fwrite failed",           //7
    "unsuppoterd compression", //8
};

const char * bmp_error(int bmp_errno)
{
    return _bmp_error_str[bmp_errno];
}

bmp_t *bmp_init()
{
    bmp_t *b = BMP_ALLOC(bmp_t, 1);
    if (NULL != b) {
        memset(b, 0, sizeof(b));
    }
    return b;
}
uint32_t bmp_data_line_length(uint32_t width, uint32_t bits_per_pixel)
{
    uint32_t bytes = (width * bits_per_pixel + 7) / 8;
    return (bytes + 3) & (~0x3);
}

int bmp_read(bmp_t *b, const char *filename)
{
    int ret;
    FILE *fp = b->fp = NULL;
    bmp_head *head = &b->head;
    bmp_info *info = &b->info;
    bmp_pale *pale = &b->pale;
    bmp_data *data = &b->data;

    fp = b->fp = fopen(filename, "rb+");
    if (NULL == fp) {
        ret = BMP_ERR_FOPEN;
        goto bmp_read_err;
    }

    if (fseek(fp, 0, SEEK_SET)) {
        ret = BMP_ERR_FSEEK;
        goto bmp_read_err;
    }

    //head
    if (fread(head, sizeof(*head), 1, fp) != 1) {
        ret = BMP_ERR_FREAD;
        goto bmp_read_err;
    }

    if (head->type[0] != 'B' || head->type[1] != 'M') {
        ret = BMP_ERR_FTYPE;
        goto bmp_read_err;
    }
    if (head->info_size != BMP_INFO_SIZE) {
        ret = BMP_ERR_INFO_SIZE;
        goto bmp_read_err;
    }

    //info
    if (fread(info, sizeof(*info), 1, fp) != 1) {
        ret = BMP_ERR_FREAD;
        goto bmp_read_err;
    }
    if (info->compression != BMP_C_NONE) {
        ret = BMP_ERR_COMPRESS;
        goto bmp_read_err;
    }

    //pale
    pale->colors = NULL;
    if (info->bits_per_pixel <= BMP_BIT8 || info->compression != BMP_C_NONE) {
        pale->color_count   = 1 << (info->bits_per_pixel);
        pale->pale_size     = sizeof(bmp_color) * pale->color_count;
        pale->colors        = BMP_ALLOC(bmp_color, pale->color_count);
        if (pale->colors == NULL) {
            ret = BMP_ERR_MALLOC;
            goto bmp_read_err;
        }
        if (fread(pale->colors, pale->pale_size, 1, fp) != 1) {
            ret = BMP_ERR_FREAD;
            goto bmp_read_err;
        }
    }
    else {
        pale->color_count   = 0;
        pale->pale_size     = 0;
    }

    //data
    data->data = NULL;
    data->data_size   = info->data_size;
    data->line_length = bmp_data_line_length(info->width, info->bits_per_pixel);

    data->data = BMP_ALLOC(uint8_t, info->data_size);
    if (data->data == NULL) {
        ret = BMP_ERR_FREAD;
        goto bmp_read_err;
    }

    if (fseek(fp, head->data_offset, SEEK_SET)) {
        ret = BMP_ERR_FSEEK;
        goto bmp_read_err;
    }

    if (fread(data->data, info->data_size, 1, fp) != 1) {
        ret = BMP_ERR_FREAD;
        goto bmp_read_err;
    }

    return 0;

bmp_read_err:
    if (NULL != fp) {
        fclose(fp);
        b->fp = NULL;
    }
    if (NULL != pale->colors) {
        free(pale->colors);
        pale->colors = NULL;
    }
    if (NULL != data->data) {
        free(data->data);
        data->data = NULL;
    }
    return ret;
}

void bmp_close(bmp_t *b)
{
    if (b)
    {
        if (b->fp)
            fclose(b->fp);
        if (b->data.data)
            free(b->data.data);
        if (b->pale.colors)
            free(b->pale.colors);
        free(b);
    }
}

void bmp_head_info_dump(bmp_t *b)
{
    bmp_head *H = &b->head;
    bmp_info *I = &b->info;

    printf(
    "HEAD:\n"
    "  [type:%c%c] [filesize:%u] [offset:%u] [info_size:0x%x]\n"
    "INFO:\n"
    "  [height:%u] [width:%u] [bits:%u] [compress:%u] [data_size:%u]\n"
    "  [h_res:%u] [v_res:%u] [colors:%u] [imp_colors:%u]\n",
    H->type[0], H->type[1], H->file_size, H->data_offset, H->info_size,
    I->height, I->width, I->bits_per_pixel, I->compression, I->data_size,
    I->horizon_reslution, I->vertical_reslution, I->colors, I->important_colors
    );
}

void bmp_pale_dump(bmp_t *b)
{
    bmp_pale *P = &b->pale;
    printf("PALETTE:\n  [color_count:%u] [pale_size:%u]\n  colors:",
            P->color_count, P->pale_size);

    int i, disp = P->color_count;
    if (disp > (1<<BMP_BIT4)) disp = (1<<BMP_BIT4); //don't display too much
    
    for (i = 0; i < disp; i++) {
        if (i % 8 == 0) printf("\n    ");
        bmp_rgb_dump(&(P->colors[i].rgb));
        printf(" ");
    }
    if (P->color_count > (1 << BMP_BIT4))
        printf("\n    ...");
    printf("\n");
}

bmp_rgb* bmp_get_rgb(bmp_t *b, int i, int j)
{
    int line = b->info.height - i - 1;
    return (bmp_rgb*) &b->data.data[line * b->data.line_length + j * sizeof(bmp_rgb)];
}

bmp_rgb *bmp_get_pale_rgb(bmp_t *b, int idx)
{
    return &b->pale.colors[idx].rgb;
}

uint8_t bmp_get_bit(bmp_t *b, int i, int j)
{
    int line = b->info.height - i - 1;
    int idx = j / 8, mask = (1 << (7 - j % 8));
    return !!(b->data.data[line * b->data.line_length + idx] & mask);
}

void bmp_set_bit(bmp_t *b, int i, int j, int value)
{
    int line = b->info.height - i - 1;
    int idx = j / 8, mask = (1 << (7 - j % 8));
    uint8_t *d = &(b->data.data[line * b->data.line_length + idx]);
    if (value) {
        *d |= mask;
    }
    else {
        *d &= (~mask);
    }
}

uint8_t bmp_get_bit4(bmp_t *b, int i, int j)
{
    int line = b->info.height - i - 1, idx = j / 2;
    int ret = b->data.data[line * b->data.line_length + idx];
    if (j % 2 == 0) { 
        return ret >> 4;
    }
    else {
        return ret & 0x0F;
    }
}

void bmp_set_bit4(bmp_t *b, int i, int j, uint8_t value)
{
    int line = b->info.height - i - 1, idx = j / 2;
    uint8_t *v = &(b->data.data[line * b->data.line_length + idx]);
    if (j % 2 == 0) {
        *v &= 0x0F;
        *v |= (value << 4);
    }
    else {
        *v &= 0xF0;
        *v |= value;
    }
}

uint8_t bmp_get_byte(bmp_t *b, int i, int j)
{
    int line = b->info.height - i - 1;
    return b->data.data[line * b->data.line_length + j];
}

void bmp_set_byte(bmp_t *b, int i, int j, uint8_t value)
{
    int line = b->info.height - i - 1;
    b->data.data[line * b->data.line_length + j] = value;
}

void bmp_rgb_dump(bmp_rgb *r)
{
    printf("%02x%02x%02x", r->red, r->green, r->blue);
}

void bmp_data_dump(bmp_t *b)
{
    bmp_pale *P = &b->pale;
    bmp_info *I = &b->info;
    bmp_data *D = &b->data;
    printf("DATA:\n  [line_len:%u]", D->line_length);
    int i, j;
    bmp_rgb *r;
    for (i = 0; i < (int)I->height; i++) {
        printf("\n  ");
        for (j = 0; j < (int)I->width; j++) {
            switch (I->bits_per_pixel)
            {
                case BMP_BIT1:
                    printf("%c", bmp_get_bit(b,i,j) ? '*' : '.');
                    break;

                case BMP_BIT4:
                    printf("[%x]", bmp_get_bit4(b, i, j));
                    bmp_rgb_dump(&P->colors[bmp_get_bit4(b, i, j)].rgb);
                    printf(" ");
                    break;
                
                case BMP_BIT8:
                    printf("[%x]", bmp_get_byte(b, i, j));
                    bmp_rgb_dump(&P->colors[bmp_get_byte(b, i, j)].rgb);
                    printf(" ");
                    break;

                case BMP_BIT24:
                case BMP_BIT32:
                    bmp_rgb_dump(bmp_get_rgb(b, i, j));
                    printf(" ");
                    break;

                case BMP_BIT16:
                default:
                    printf("[==== NOT SUPPORTED YET ====]\n");
                    return;
            }
        }
    }
    printf("\n");
}

void print_uint8(uint8_t v)
{
    int i;
    for (i = 7; i >= 0; i--) {
        printf("%d", (v >> i) & 1);
    }
}

void bmp_data_dump_raw(bmp_t *b)
{
    bmp_pale *P = &b->pale;
    bmp_info *I = &b->info;
    bmp_data *D = &b->data;
    printf("DATA:\n  [line_len:%u]", D->line_length);
    int i, j;
    bmp_rgb *r;
    for (i = 0; i < (int)I->height; i++) {
        int line = I->height - i - 1;
        printf("\n  ");
        for (j = 0; j < D->line_length; j++) {
            print_uint8(D->data[line * D->line_length + j]);
            printf(" ");
        }
    }
    printf("\n");
}

bmp_rgb bmp_make_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    bmp_rgb rgb = {b, g, r};
    return rgb;
}

void bmp_dump(bmp_t *b)
{
    bmp_head_info_dump(b);
    bmp_pale_dump(b);
    bmp_data_dump(b);
}

int bmp_write(bmp_t *b)
{
    int ret = 0;
    FILE *fp = b->fp;
    bmp_head *H = &b->head;
    bmp_info *I = &b->info;
    bmp_pale *P = &b->pale;
    bmp_data *D = &b->data;

    if (fseek(fp, 0, SEEK_SET)) {
        return BMP_ERR_FSEEK;
    }

    //head
    if (fwrite(H, sizeof(*H), 1, fp) != 1) {
        return BMP_ERR_FWRITE;
    }

    //info
    if (fwrite(I, sizeof(*I), 1, fp) != 1) {
        return BMP_ERR_FWRITE;
    }

    //pale
    if (P->pale_size > 0) {
        if (fwrite(P->colors, P->pale_size, 1, fp) != 1) {
            return BMP_ERR_FWRITE;
        }
    }

    //data
    if (fseek(fp, H->data_offset, SEEK_SET)) {
        return BMP_ERR_FSEEK;
    }

    if (fwrite(D->data, D->data_size, 1, fp) != 1) {
        return BMP_ERR_FWRITE;
    }

    return 0;
}

int bmp_create(bmp_t *b, char *filename,
        uint32_t width, uint32_t height, uint16_t bits_per_pixel)
{
    int ret = 0, i;
    FILE *fp = b->fp = NULL;
    bmp_head *H = &b->head;
    bmp_info *I = &b->info;
    bmp_pale *P = &b->pale;
    bmp_data *D = &b->data;

    for (i = 0; i < n_bits_supported; i++) {
        if (bits_per_pixel == bits_supported[i])
            break;
    }
    if (i == n_bits_supported) {
        ret = BMP_ERR_BADBITS;
        goto bmp_create_err;
    }

    fp = b->fp = fopen(filename, "wb+");
    if (fp == NULL) {
        ret = BMP_ERR_FOPEN;
        goto bmp_create_err;
    }

    //pale
    if (bits_per_pixel <= BMP_BIT8) {
        P->color_count  = (1 << bits_per_pixel);
        P->pale_size    = P->color_count * sizeof(bmp_color);
        P->colors       = NULL;

        P->colors       = BMP_ALLOC(bmp_color, P->color_count);
        if (NULL == P->colors) {
            ret = BMP_ERR_MALLOC;
            goto bmp_create_err;
        }

        if (P->color_count == 2) {
            *bmp_get_pale_rgb(b, 0) = BMP_RGB_BLACK;
            *bmp_get_pale_rgb(b, 1) = BMP_RGB_WHITE;
        }
        else {
            memset(P->colors, 0, sizeof(P->pale_size));
        }
    }
    else {
        P->color_count  = 0;
        P->pale_size    = 0;
        P->colors       = NULL;
    }

    //data
    D->line_length  = bmp_data_line_length(width, bits_per_pixel);
    D->data_size    = D->line_length * height;
    D->data         = NULL;

    D->data = BMP_ALLOC(uint8_t, D->data_size);
    if (NULL == D->data) {
        ret = BMP_ERR_MALLOC;
        goto bmp_create_err;
    }
    memset(D->data, 0xff, D->data_size);

    //info
    bmp_info Ix = {
        width, height, BMP_PLANES, bits_per_pixel, BMP_C_NONE,
        D->data_size, BMP_HRESO, BMP_VRESO, 0, 0
    };
    *I = Ix;

    //head
    bmp_head Hx = {{'B', 'M'}, 0, BMP_RESERVED, 0, BMP_INFO_SIZE};
    *H = Hx;
    H->data_offset  = sizeof(*H) + sizeof(*I) + P->pale_size;
    H->file_size    = H->data_offset + D->data_size;

    return 0;

bmp_create_err:
    if (NULL != P->colors)
        free(P->colors);
    if (NULL != D->data)
        free(D->data);
    return ret;
}


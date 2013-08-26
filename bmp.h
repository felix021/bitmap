#ifndef BMP_H
#define BMP_H

#ifndef uint32_t
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
#endif

#pragma pack(2)

#define BMP_TYPE_BM     0x4d42
#define BMP_INFO_SIZE   0x28
#define BMP_RESERVED    0

struct bmp_head
{
    char        type[2];
    uint32_t    file_size;
    uint32_t    reserved;
    uint32_t    data_offset;
    uint32_t    info_size;
};
typedef struct bmp_head bmp_head;

#define BMP_BIT1    1
#define BMP_BIT4    4
#define BMP_BIT8    8
#define BMP_BIT16   16
#define BMP_BIT24   24
#define BMP_BIT32   32

#define BMP_C_NONE  0
#define BMP_C_RLE4  1
#define BMP_C_RLE8  2
#define BMP_C_BITF  3

#define BMP_PLANES  1
#define BMP_HRESO   0
#define BMP_VRESO   0
#define BMP_COLORS  0

struct bmp_info
{
    uint32_t    width;
    uint32_t    height;
    uint16_t    plane; //=1
    uint16_t    bits_per_pixel;
    uint32_t    compression;
    uint32_t    data_size;
    uint32_t    horizon_reslution;
    uint32_t    vertical_reslution;
    uint32_t    colors;
    uint32_t    important_colors;
};
typedef struct bmp_info bmp_info;

#pragma pack(1)
struct bmp_rgb
{
    uint8_t     blue;
    uint8_t     green;
    uint8_t     red;
};
typedef struct bmp_rgb bmp_rgb;

struct bmp_color
{
    bmp_rgb     rgb;
    uint8_t      none;
};
typedef struct bmp_color bmp_color;
#pragma pack(2)

struct bmp_pale
{
    uint32_t    pale_size;
    uint32_t    color_count;
    bmp_color*  colors;
};
typedef struct bmp_pale bmp_pale;

struct bmp_data
{
    uint32_t    data_size;
    uint32_t    line_length;
    uint8_t*    data;
};
typedef struct bmp_data bmp_data;

struct bmp_t
{
    FILE *      fp;
    bmp_head    head;
    bmp_info    info;
    bmp_pale    pale;
    bmp_data    data;
};
typedef struct bmp_t bmp_t;

#define BMP_ALLOC(type, n)  ((type*)malloc(sizeof(type) * n))

#define BMP_ERR_FOPEN       1
#define BMP_ERR_FSEEK       2
#define BMP_ERR_FREAD       3
#define BMP_ERR_FTYPE       4
#define BMP_ERR_INFO_SIZE   5
#define BMP_ERR_MALLOC      6
#define BMP_ERR_FWRITE      7
#define BMP_ERR_COMPRESS    8
#define BMP_ERR_BADBITS     9

bmp_t *bmp_init();
int bmp_read(bmp_t *b, const char *filename);
int bmp_create(bmp_t *b, char *filename,
        uint32_t width, uint32_t height, uint16_t bits_per_pixel);
int bmp_write(bmp_t *b);
void bmp_close(bmp_t *b);

void bmp_rgb_dump(bmp_rgb *r);
void bmp_head_info_dump(bmp_t *b);
void bmp_pale_dump(bmp_t *b);
void bmp_data_dump(bmp_t *b);
void bmp_dump(bmp_t *b);
const char * bmp_error(int bmp_errno);

bmp_rgb bmp_make_rgb(uint8_t r, uint8_t g, uint8_t b);

uint8_t bmp_get_bit(bmp_t *b, int i, int j);
void bmp_set_bit(bmp_t *b, int i, int j, int value);

uint8_t bmp_get_bit4(bmp_t *b, int i, int j);
void bmp_set_bit4(bmp_t *b, int i, int j, uint8_t value);

uint8_t bmp_get_byte(bmp_t *b, int i, int j);
void bmp_set_byte(bmp_t *b, int i, int j, uint8_t value);

bmp_rgb* bmp_get_rgb(bmp_t *b, int i, int j);
bmp_rgb *bmp_get_pale_rgb(bmp_t *b, int idx);

extern const bmp_rgb BMP_RGB_WHITE;
extern const bmp_rgb BMP_RGB_BLACK;
extern const bmp_rgb BMP_RGB_RED;
extern const bmp_rgb BMP_RGB_GREEN;
extern const bmp_rgb BMP_RGB_BLUE;

#endif

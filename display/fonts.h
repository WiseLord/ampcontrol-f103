#ifndef FONTS_H
#define FONTS_H

#include <inttypes.h>

#define LETTER_SPACE_CHAR   0x00E28088  // U+2008
#define BLOCK_CHAR          0x00E29688  // U+2588

typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
} tImage;

typedef struct {
    int32_t code;
    const tImage *image;
} tChar;

typedef struct {
    uint16_t length;
    const tChar *chars;
} tFont;

extern const tFont fontamp08;
extern const tFont fontamp24;
extern const tFont fontampdig32;

typedef struct {
    const tFont *tfont;
    uint16_t color;

    uint8_t fixed;
    uint8_t mult;
    uint8_t direction;
} Font;

#endif // FONTS_H

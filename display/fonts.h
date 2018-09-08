#ifndef FONTS_H
#define FONTS_H

#include <inttypes.h>

#define LETTER_SPACE_CHAR   0xA0

typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

typedef struct {
    uint16_t code;
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

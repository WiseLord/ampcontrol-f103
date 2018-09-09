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

extern const tFont fontterminus14;
extern const tFont fontterminus14b;
extern const tFont fontterminus16;
extern const tFont fontterminus16b;
extern const tFont fontterminus18;
extern const tFont fontterminus18b;
extern const tFont fontterminus20;
extern const tFont fontterminus20b;
extern const tFont fontterminus22;
extern const tFont fontterminus22b;
extern const tFont fontterminus28;
extern const tFont fontterminus28b;
extern const tFont fontterminus32;
extern const tFont fontterminus32b;


extern const tFont fontterminus12;
extern const tFont fontterminus24;
extern const tFont fontterminus24b;
extern const tFont fontterminus22dig;
extern const tFont fontterminus30dig;

extern const tFont fontterminus64;
extern const tFont fontterminus64dig;
extern const tFont fontterminus80dig;

typedef enum {
    FONT_ALIGN_LEFT,
    FONT_ALIGN_CENTER,
    FONT_ALIGN_RIGHT
} FontAlign;

typedef struct {
    const tFont *tfont;
    uint16_t color;

    uint8_t align;
    uint8_t fixed;
    uint8_t mult;
    uint8_t direction;
} Font;

#endif // FONTS_H

#ifndef FONTS_H
#define FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LETTER_SPACE_CHAR   0x00E28088  // U+2008
#define BLOCK_CHAR          0x00E29688  // U+2588

typedef struct {
    const uint8_t *data;
    int16_t width;
    int16_t height;
    uint16_t size;
    uint8_t rle;
} tImage;

typedef struct {
    int32_t code;
    const tImage *image;
} tChar;

typedef struct {
    uint16_t length;
    const tChar *chars;
} tFont;

typedef enum {
    FONT_ALIGN_LEFT,
    FONT_ALIGN_CENTER,
    FONT_ALIGN_RIGHT
} FontAlign;

typedef struct {
    const tFont *tfont;
    uint16_t color;
    uint16_t bgColor;

    uint8_t align;
} Font;

// Originally exported fonts
extern const tFont fontterminus12;
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
extern const tFont fontterminus24;
extern const tFont fontterminus24b;
extern const tFont fontterminus28;
extern const tFont fontterminus28b;
extern const tFont fontterminus32;
extern const tFont fontterminus32b;
// Modified fonts - full character set
extern const tFont fontterminusmod18x3;
extern const tFont fontterminusmod24x3;
// Modified fonts - for digits only
extern const tFont fontterminusdig30;
extern const tFont fontterminusdig40;
extern const tFont fontterminusdig58;
extern const tFont fontterminusdig64;
extern const tFont fontterminusdig80;
extern const tFont fontterminusdig96;
extern const tFont fontterminusdig120;

#ifdef __cplusplus
}
#endif

#endif // FONTS_H

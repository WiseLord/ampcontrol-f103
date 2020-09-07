#ifndef FONTS_H
#define FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define __flash
#endif

#include <stdint.h>

#define LETTER_SPACE_CHAR   0x00E28088  // U+2008
#define BLOCK_CHAR          0x00E29688  // U+2588

typedef struct {
    const __flash uint8_t *data;
    int16_t width;
    int16_t height;
    uint16_t size;
    uint8_t rle;
} tImage;

typedef struct {
    int32_t code;
    const __flash tImage *image;
} tChar;

typedef struct {
    uint16_t length;
    const __flash tChar *chars;
} tFont;

// Originally exported fonts
extern const __flash tFont fontterminus12;
extern const __flash tFont fontterminus14;
extern const __flash tFont fontterminus14b;
extern const __flash tFont fontterminus16;
extern const __flash tFont fontterminus16b;
extern const __flash tFont fontterminus18;
extern const __flash tFont fontterminus18b;
extern const __flash tFont fontterminus20;
extern const __flash tFont fontterminus20b;
extern const __flash tFont fontterminus22;
extern const __flash tFont fontterminus22b;
extern const __flash tFont fontterminus24;
extern const __flash tFont fontterminus24b;
extern const __flash tFont fontterminus28;
extern const __flash tFont fontterminus28b;
extern const __flash tFont fontterminus32;
extern const __flash tFont fontterminus32b;

#ifdef __cplusplus
}
#endif

#endif // FONTS_H

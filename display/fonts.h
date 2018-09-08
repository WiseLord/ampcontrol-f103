#ifndef FONTS_H
#define FONTS_H

#include <inttypes.h>

typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

typedef struct {
    long int code;
    const tImage *image;
} tChar;

typedef struct {
    int length;
    const tChar *chars;
} tFont;

extern const tFont fontamp08;
extern const tFont fontamp24;
extern const tFont fontampdig32;

typedef enum {
    FONT_HEIGHT = 0,
    FONT_LTSPPOS,
    FONT_CCNT,
    FONT_OFTA,
    FONT_OFTNA,

    FONT_HEADER_END
} FontHeader;

typedef struct {
    const uint8_t *data;

    uint16_t color;

    const tFont *tfont;

    uint8_t fixed;
    uint8_t mult;
    uint8_t direction;
} Font;

extern const uint8_t font_ks0066_ru_08[];
extern const uint8_t font_ks0066_ru_24[];
extern const uint8_t font_digits_32[];

#endif // FONTS_H

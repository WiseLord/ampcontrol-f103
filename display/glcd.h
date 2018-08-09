#ifndef GLCD_H
#define GLCD_H

#include <inttypes.h>
#include "../rtc.h"

typedef struct {
    const char *label;
    int16_t value;
    int16_t min;
    int16_t max;
    uint8_t icon;
} DispParam;

typedef struct {
    uint16_t width;
    uint16_t height;
    int16_t x;
    int16_t y;

    void (*showTime)(RTC_type *rtc, char *wday);
    void (*showParam)(DispParam *dp);
    void (*showSpectrum)(uint8_t *dataL, uint8_t *dataR);
} DisplayLayout;

typedef struct {
    void (*setBrightness)(uint8_t br);
    uint8_t (*readBus)(void);
    void (*drawPixel)(int16_t x, int16_t y, uint16_t color);
    DisplayLayout *layout;
} DisplayDriver;

enum {
    FONT_HEIGHT = 0,
    FONT_LTSPPOS,
    FONT_CCNT,
    FONT_OFTA,
    FONT_OFTNA,
    FONT_COLOR,

    FONT_END
};

enum {
    FONT_DIRECTION = FONT_END,
    FONT_FIXED,

    FONT_PARAM_END
};

enum {
    FONT_DIR_0,
    FONT_DIR_90,
    FONT_DIR_180,
    FONT_DIR_270
};

void glcdInit(DisplayDriver *driver);

void glcdWriteNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix);

void glcdLoadFont(const uint8_t *font, uint16_t color, uint8_t direction);

void glcdSetXY(int16_t x, int16_t y);

void glcdWriteChar(uint8_t code);
void glcdWriteString(char *string);

#endif // GLCD_H
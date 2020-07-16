#ifndef GLCD_H
#define GLCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "colors.h"
#include "dispdrv.h"
#include "fonts.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} GlcdRect;

typedef uint8_t GlcdAlign;
enum {
    GLCD_ALIGN_LEFT,
    GLCD_ALIGN_CENTER,
    GLCD_ALIGN_RIGHT
};

typedef uint8_t GlcdOrientation;
enum {
    GLCD_LANDSCAPE      = 0x00,
    GLCD_LANDSCAPE_ROT  = 0x01,
    GLCD_PORTRATE       = 0x02,
    GLCD_PORTRATE_ROT   = 0x03,
};

typedef struct {
    const DispDriver *drv;
    const tFont *font;
    GlcdRect rect;
    int16_t x;
    int16_t y;
    color_t fontFg;
    color_t fontBg;
    GlcdAlign fontAlign;
    GlcdOrientation orientation;
    bool strFramed;
} Glcd;

typedef int32_t UChar;

void glcdInit(GlcdOrientation value);

void glcdSetBacklight(bool value);

Glcd *glcdGet(void);

void glcdSetOrientation(GlcdOrientation value);
void glcdShift(int16_t pos);
void glcdSleep(bool value);
void glcdSetIdle(bool value);

void glcdFbSync(void);

void glcdSetRect(const GlcdRect *rect);
void glcdSetRectValues(int16_t x, int16_t y, int16_t w, int16_t h);
void glcdResetRect(void);
GlcdRect *glcdGetRect(void);

void glcdSetFont(const tFont *font);
void glcdSetFontColor(color_t color);
void glcdSetFontBgColor(color_t color);
void glcdSetFontAlign(GlcdAlign align);
int16_t glcdGetFontHeight(const tFont *font);

void glcdSetXY(int16_t x, int16_t y);
void glcdSetX(int16_t x);
void glcdSetY(int16_t y);

int16_t glcdFontSymbolPos(UChar code);
UChar glcdFontSymbolCode(int16_t pos);

void glcdDrawImage(const tImage *img, color_t color, color_t bgColor);

uint16_t glcdStrToUStr(const char *str, UChar *ustr);
void glcdUStrToStr(const UChar *ustr, char *str);
int16_t glcdWriteUChar(UChar code);
int16_t glcdCalcUCharLen(UChar code);

void glcdSetStringFramed(bool framed);

int16_t glcdWriteString(const char *string);
int16_t glcdCalcStringLen(const char *string);

void glcdDrawPixel(int16_t x, int16_t y, color_t color);

void glcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color);

void glcdDrawVertGrad(int16_t x, int16_t y, int16_t w, int16_t h, color_t *gr);

void glcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color);

void glcdDrawFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, color_t color);
void glcdDrawRFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, int16_t r,
                    color_t color);

void glcdDrawCircle(int16_t xc, int16_t yc, int16_t r, color_t color);
void glcdDrawRing(int16_t xc, int16_t yc, int16_t r, int16_t t, color_t color);

#ifdef __cplusplus
}
#endif

#endif // GLCD_H

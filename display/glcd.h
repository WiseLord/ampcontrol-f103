#ifndef GLCD_H
#define GLCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "fonts.h"
#include "dispdrv.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} GlcdRect;

typedef enum {
    GLCD_ALIGN_LEFT,
    GLCD_ALIGN_CENTER,
    GLCD_ALIGN_RIGHT
} GlcdAlign;

typedef struct {
    const DispDriver *drv;
    const tFont *font;
    GlcdRect rect;
    int16_t x;
    int16_t y;
    color_t fontFg;
    color_t fontBg;
    uint8_t fontAlign;
    bool rotate;
    bool strFramed;
} Glcd;

typedef int32_t UChar;

//Colors
#define RGB_R(x)                        (((x) >> 16) & 0xFF)
#define RGB_G(x)                        (((x) >> 8) & 0xFF)
#define RGB_B(x)                        (((x) >> 0) & 0xFF)

// RGB_CONV
#ifdef _SSD1322
#define RGB_CONV(x)                     ((uint8_t)(RGB_R(x) > RGB_G(x) \
                                            ? (RGB_R(x) > RGB_B(x) ? (RGB_R(x) >> 4) : (RGB_B(x) >> 4)) \
                                            : (RGB_G(x) > RGB_B(x) ? (RGB_G(x) >> 4) : (RGB_B(x) >> 4))))
#else
#define RGB_CONV(x)                     ((uint16_t)(((RGB_R(x) & 0xF8) << 8) | \
                                                    ((RGB_G(x) & 0xFC) << 3)) | \
                                                    ((RGB_B(x) & 0xF8) >> 3))
#endif

#define LCD_COLOR_BLACK                 RGB_CONV(0x000000)
#define LCD_COLOR_NAVI                  RGB_CONV(0x000080)
#define LCD_COLOR_BLUE                  RGB_CONV(0x0000FF)
#define LCD_COLOR_GREEN                 RGB_CONV(0x008000)
#define LCD_COLOR_TEAL                  RGB_CONV(0x008080)
#define LCD_COLOR_DODGER_BLUE           RGB_CONV(0x0080FF)
#define LCD_COLOR_LIME                  RGB_CONV(0x00FF00)
#define LCD_COLOR_SPRING_GREEN          RGB_CONV(0x00FF80)
#define LCD_COLOR_AQUA                  RGB_CONV(0x00FFFF)
#define LCD_COLOR_NERO                  RGB_CONV(0x252525)
#define LCD_COLOR_ECLIPSE               RGB_CONV(0x3F3939)
#define LCD_COLOR_MAROON                RGB_CONV(0x800000)
#define LCD_COLOR_PURPLE                RGB_CONV(0x800080)
#define LCD_COLOR_ELECTRIC_INDIGO       RGB_CONV(0x8000FF)
#define LCD_COLOR_OLIVE                 RGB_CONV(0x808000)
#define LCD_COLOR_GRAY                  RGB_CONV(0x808080)
#define LCD_COLOR_LIGHT_SLATE_BLUE      RGB_CONV(0x8080FF)
#define LCD_COLOR_CHARTREUSE            RGB_CONV(0x80FF00)
#define LCD_COLOR_LIGHT_GREEN           RGB_CONV(0x80FF80)
#define LCD_COLOR_ELECTRIC_BLUE         RGB_CONV(0x80FFFF)
#define LCD_COLOR_RED                   RGB_CONV(0xFF0000)
#define LCD_COLOR_DEEP_PINK             RGB_CONV(0xFF0080)
#define LCD_COLOR_MAGENTA               RGB_CONV(0xFF00FF)
#define LCD_COLOR_DARK_ORANGE           RGB_CONV(0xFF8000)
#define LCD_COLOR_LIGHT_CORAL           RGB_CONV(0xFF8080)
#define LCD_COLOR_FUCHSIA_PINK          RGB_CONV(0xFF80FF)
#define LCD_COLOR_YELLOW                RGB_CONV(0xFFFF00)
#define LCD_COLOR_WITCH_HAZE            RGB_CONV(0xFFFF80)
#define LCD_COLOR_WHITE                 RGB_CONV(0xFFFFFF)

void glcdInit(Glcd **value);

bool glcdGetRotate(void);
void glcdRotate(uint8_t rotate);
void glcdShift(int16_t pos);

void glcdFbSync(void);

void glcdSetRect(const GlcdRect *rect);
GlcdRect *glcdGetRect(void);

void glcdSetFont(const tFont *font);
void glcdSetFontColor(color_t color);
void glcdSetFontBgColor(color_t color);
void glcdSetFontAlign(uint8_t align);
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

void glcdSetStringFramed(bool framed);

uint16_t glcdWriteString(const char *string);

void glcdDrawPixel(int16_t x, int16_t y, color_t color);

void glcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color);

void glcdDrawVertGrad(int16_t x, int16_t y, int16_t w, int16_t h, color_t *gr);

void glcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color);

void glcdDrawFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, color_t color);
void glcdDrawRFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, int16_t r, color_t color);

void glcdDrawCircle(int16_t xc, int16_t yc, int16_t r, color_t color);
void glcdDrawRing(int16_t xc, int16_t yc, int16_t r, int16_t t, color_t color);

#ifdef __cplusplus
}
#endif

#endif // GLCD_H

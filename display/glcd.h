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
    uint16_t fontFg;
    uint16_t fontBg;
    uint8_t fontAlign;
    bool rotate;
    bool strFramed;
} Glcd;

typedef int32_t UChar;
typedef uint16_t uint16_t;

//Colors
#define RGB_TO_565(x)                   ((uint16_t)(((x >> 8) & 0xF800) | ((x >> 5) & 0x7E0) | ((x >> 3) & 0x1F)))
#define LCD_COLOR_BLACK                 RGB_TO_565(0x000000)
#define LCD_COLOR_NAVI                  RGB_TO_565(0x000080)
#define LCD_COLOR_BLUE                  RGB_TO_565(0x0000FF)
#define LCD_COLOR_GREEN                 RGB_TO_565(0x008000)
#define LCD_COLOR_TEAL                  RGB_TO_565(0x008080)
#define LCD_COLOR_DODGER_BLUE           RGB_TO_565(0x0080FF)
#define LCD_COLOR_LIME                  RGB_TO_565(0x00FF00)
#define LCD_COLOR_SPRING_GREEN          RGB_TO_565(0x00FF80)
#define LCD_COLOR_AQUA                  RGB_TO_565(0x00FFFF)
#define LCD_COLOR_NERO                  RGB_TO_565(0x252525)
#define LCD_COLOR_ECLIPSE               RGB_TO_565(0x3F3939)
#define LCD_COLOR_MAROON                RGB_TO_565(0x800000)
#define LCD_COLOR_PURPLE                RGB_TO_565(0x800080)
#define LCD_COLOR_ELECTRIC_INDIGO       RGB_TO_565(0x8000FF)
#define LCD_COLOR_OLIVE                 RGB_TO_565(0x808000)
#define LCD_COLOR_GRAY                  RGB_TO_565(0x808080)
#define LCD_COLOR_LIGHT_SLATE_BLUE      RGB_TO_565(0x8080FF)
#define LCD_COLOR_CHARTREUSE            RGB_TO_565(0x80FF00)
#define LCD_COLOR_LIGHT_GREEN           RGB_TO_565(0x80FF80)
#define LCD_COLOR_ELECTRIC_BLUE         RGB_TO_565(0x80FFFF)
#define LCD_COLOR_RED                   RGB_TO_565(0xFF0000)
#define LCD_COLOR_DEEP_PINK             RGB_TO_565(0xFF0080)
#define LCD_COLOR_MAGENTA               RGB_TO_565(0xFF00FF)
#define LCD_COLOR_DARK_ORANGE           RGB_TO_565(0xFF8000)
#define LCD_COLOR_LIGHT_CORAL           RGB_TO_565(0xFF8080)
#define LCD_COLOR_FUCHSIA_PINK          RGB_TO_565(0xFF80FF)
#define LCD_COLOR_YELLOW                RGB_TO_565(0xFFFF00)
#define LCD_COLOR_WITCH_HAZE            RGB_TO_565(0xFFFF80)
#define LCD_COLOR_WHITE                 RGB_TO_565(0xFFFFFF)

void glcdInit(Glcd **value);

bool glcdGetRotate(void);
void glcdRotate(uint8_t rotate);
void glcdShift(int16_t pos);

void glcdSetRect(const GlcdRect *rect);
GlcdRect *glcdGetRect(void);

void glcdSetFont(const tFont *font);
void glcdSetFontColor(uint16_t color);
void glcdSetFontBgColor(uint16_t color);
void glcdSetFontAlign(uint8_t align);
int16_t glcdGetFontHeight(const tFont *font);

void glcdSetXY(int16_t x, int16_t y);
void glcdSetX(int16_t x);
void glcdSetY(int16_t y);

int16_t glcdFontSymbolPos(UChar code);
UChar glcdFontSymbolCode(int16_t pos);

void glcdDrawImage(const tImage *img, uint16_t color, uint16_t bgColor);

uint16_t glcdStrToUStr(const char *str, UChar *ustr);
void glcdUStrToStr(const UChar *ustr, char *str);
int16_t glcdWriteUChar(UChar code);

void glcdSetStringFramed(bool framed);

uint16_t glcdWriteString(const char *string);

void glcdDrawPixel(int16_t x, int16_t y, uint16_t color);

void glcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void glcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

void glcdDrawFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, uint16_t color);
void glcdDrawRFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, int16_t r, uint16_t color);

void glcdDrawCircle(int16_t xc, int16_t yc, int16_t r, uint16_t color);
void glcdDrawRing(int16_t xc, int16_t yc, int16_t r, int16_t t, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif // GLCD_H

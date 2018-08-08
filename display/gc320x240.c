#include "gc320x240.h"

#include "ili9320.h"

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(uint8_t *dataL, uint8_t *dataR);

Display gc320x240 = {
    .width = 320,
    .height = 240,
//    .writeChar = gdWriteChar,
//    .writeString = gdWriteString,
//    .setBrightness = ks0108SetBrightness,
    .readBus = ILI9320_GetPins,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

static void showTime(RTC_type *rtc, char *wday)
{
    ILI9320_DrawFilledCircle(160, 120, 59, LCD_COLOR_RED);
}

static void showParam(DispParam *dp)
{
    ILI9320_DrawFilledCircle(160, 120, 59, LCD_COLOR_GREEN);
}

static void showSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    ILI9320_DrawFilledCircle(160, 120, 59, LCD_COLOR_YELLOW);
}

void gc320x240Init(Display **disp)
{
    *disp = &gc320x240;

    ILI9320_Init();
    ILI9320_Rotate(LCD_Orientation_Landscape_1);

    ILI9320_DrawFilledRectangle(0, 0, 319, 239, LCD_COLOR_BLACK);
}

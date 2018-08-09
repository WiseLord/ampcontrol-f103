#include "gc320x240.h"

#include "ili9320.h"

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(uint8_t *dataL, uint8_t *dataR);

DisplayDriver *disp;

DisplayLayout gc320x240 = {
    .width = 320,
    .height = 240,

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

void gc320x240Init(DisplayDriver *driver)
{
    disp = driver;
    disp->layout = &gc320x240;
    glcdInit(disp);
}

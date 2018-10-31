#include "canvas.h"

// On 400x240 we can draw max 8 menu items + menu header
#define MENU_SIZE_VISIBLE   8

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);
static void showMenu(void);

static Canvas canvas = {
    .width = 400,
    .height = 240,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,
    .showMenu = showMenu,
};

static CanvasBar canvasBar = {
    .sc = 80,
    .sw = 3,
    .pos = 84,
    .half = 14,
    .middle = 2,
};

static const CanvasTime canvasTime = {
    .hmsFont = &fontterminusdig80,
    .dmyFont = &fontterminusdig64,
    .wdFont = &fontterminusmod64,
    .hmsY = 10,
    .dmyY = 100,
    .wdY = 170,
};

void gc400x240Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = MENU_SIZE_VISIBLE;
}

static void showTime(bool clear, RTC_type *rtc)
{
    canvasShowTime(clear, &canvasTime, rtc);
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminusmod64);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max, &canvasBar);

    glcdSetXY(canvas.width, 160);
    glcdSetFont(&fontterminusdig80);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 4;     // Step in pixels between spectrum columns
    const uint8_t oft = 1;      // Offset of spectrum column inside step

    const uint8_t width = 2;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminusmod64;

    canvasShowTuner(dt, fmFont, &canvasBar);
}

static void showMenu(void)
{
    canvasShowMenu(&fontterminus28b, &fontterminus22b);
}

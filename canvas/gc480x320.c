#include "canvas.h"

// On 320x240 we can draw max 10 menu items + menu header
#define MENU_SIZE_VISIBLE   10

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);
static void showMenu(void);

static Canvas canvas = {
    .width = 480,
    .height = 320,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,
    .showMenu = showMenu,
};

static CanvasBar canvasBar = {
    .sc = 80,
    .sw = 3,
    .pos = 110,
    .half = 14,
    .middle = 2,
};

static const CanvasTime canvasTime = {
    .hmsFont = &fontterminusdig120,
    .dmyFont = &fontterminusdig96,
    .wdFont = &fontterminusmod96,
    .hmsY = 4,
    .dmyY = 124,
    .wdY = 224,
};

void gc480x320Init(Canvas **value)
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
    glcdSetFont(&fontterminusmod96);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max, &canvasBar);

    glcdSetXY(canvas.width, 224);
    glcdSetFont(&fontterminusdig96);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 5;     // Step in pixels between spectrum columns
    const uint8_t oft = 1;      // Offset of spectrum column inside step

    const uint8_t width = 3;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminusmod96;

    canvasShowTuner(dt, fmFont, &canvasBar);
}

static void showMenu(void)
{
    canvasShowMenu(&fontterminus32b, &fontterminus24b);
}

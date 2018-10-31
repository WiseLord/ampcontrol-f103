#include "canvas.h"

// On 160x128 we can draw max 4 menu items + menu header
#define MENU_SIZE_VISIBLE   7

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);
static void showMenu(void);

static Canvas canvas = {
    .width = 160,
    .height = 128,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,
    .showMenu = showMenu,
};

static CanvasBar canvasBar = {
    .sc = 80,
    .sw = 1,
    .pos = 32,
    .half = 6,
    .middle = 2,
};

static const CanvasTime canvasTime = {
    .hmsFont = &fontterminusdig40,
    .dmyFont = &fontterminusdig30,
    .wdFont = &fontterminus24b,
    .hmsY = 8,
    .dmyY = 60,
    .wdY = 96,
};

void gc160x128Init(Canvas **value)
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
    glcdSetFont(&fontterminus28);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max, &canvasBar);

    glcdSetXY(canvas.width, 88);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdSetFont(&fontterminusdig40);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 2;     // Step in pixels between spectrum columns
    const uint8_t oft = 0;      // Offset of spectrum column inside step

    const uint8_t width = 1;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminus32;

    canvasShowTuner(dt, fmFont, &canvasBar);
}

static void showMenu(void)
{
    canvasShowMenu(&fontterminus14b, &fontterminus12);
}

#include "canvas.h"

// On 128x64 we can draw max 4 menu items + menu header
#define MENU_SIZE_VISIBLE   3

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);
static void showMenu(void);

static Canvas canvas = {
    .width = 128,
    .height = 64,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,
    .showMenu = showMenu,
};

static CanvasBar canvasBar = {
    .sc = 64,
    .sw = 1,
    .pos = 26,
    .half = 4,
    .middle = 2,
};

static const CanvasTime canvasTime = {
    .hmsFont = &fontterminusdig30,
    .dmyFont = &fontterminusdig22,
    .wdFont = &fontterminus12,
    .hmsY = 0,
    .dmyY = 30,
    .wdY = 52,
};

void gm128x64Init(Canvas **value)
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
    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(0, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max, &canvasBar);

    glcdSetXY(canvas.width, 30);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdSetFont(&fontterminus12);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);

    glcdSetXY(104, 2);
    glcdWriteIcon(dp->icon, icons_24, LCD_COLOR_WHITE, canvas.color);
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
    const tFont *fmFont = &fontterminus24;

    canvasShowTuner(dt, fmFont, &canvasBar);
}

static void showMenu(void)
{
    canvasShowMenu(&fontterminus14b, &fontterminus12);
}

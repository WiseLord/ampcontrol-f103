#include "canvas.h"

// On 320x240 we can draw max 10 menu items + menu header
#define MENU_SIZE_VISIBLE   10

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);
static void showMenu(void);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 480,
    .height = 320,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,
    .showMenu = showMenu,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig120,
    .time.dmyFont = &fontterminusdig96,
    .time.wdFont = &fontterminusmod96,
    .time.hmsY = 4,
    .time.dmyY = 124,
    .time.wdY = 224,

    .bar.sc = 80,
    .bar.sw = 3,
    .bar.pos = 110,
    .bar.half = 14,
    .bar.middle = 2,

    .menu.headFont = &fontterminus32b,
    .menu.menuFont = &fontterminus24b,
};

void gc480x320Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = MENU_SIZE_VISIBLE;
}

static void showTime(bool clear, RTC_type *rtc)
{
    canvasShowTime(clear, rtc);
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminusmod96);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max);

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

    canvasShowTuner(dt, fmFont);
}

static void showMenu(void)
{
    canvasShowMenu();
}

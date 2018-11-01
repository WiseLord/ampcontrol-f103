#include "canvas.h"

// On 320x240 we can draw max 8 menu items + menu header
#define MENU_SIZE_VISIBLE   8

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);
static void showMenu(void);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 320,
    .height = 240,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,
    .showMenu = showMenu,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod64,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .bar.sc = 80,
    .bar.sw = 2,
    .bar.pos = 84,
    .bar.half = 12,
    .bar.middle = 2,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
};

void gc320x240Init(Canvas **value)
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
    glcdSetFont(&fontterminusmod64);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max);

    glcdSetXY(canvas.width, 176);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdSetFont(&fontterminusdig64);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 3;     // Step in pixels between spectrum columns
    const uint8_t oft = 0;      // Offset of spectrum column inside step

    const uint8_t width = 2;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminusmod64;

    canvasShowTuner(dt, fmFont);
}

static void showMenu(void)
{
    canvasShowMenu();
}

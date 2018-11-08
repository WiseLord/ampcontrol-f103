#include "canvas.h"

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 220,
    .height = 176,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig58,
    .time.dmyFont = &fontterminusdig40,
    .time.wdFont = &fontterminus32,
    .time.hmsY = 10,
    .time.dmyY = 78,
    .time.wdY = 126,

    .bar.sc = 73,
    .bar.sw = 2,
    .bar.pos = 54,
    .bar.half = 10,
    .bar.middle = 2,

    .menu.headFont = &fontterminus20b,
    .menu.menuFont = &fontterminus18,
    .menu.itemCnt = 7,
};

void gc220x176Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = canvas.par->menu.itemCnt;
}

static void showTime(bool clear, RTC_type *rtc)
{
    canvasShowTime(clear, rtc);
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminus32);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max);

    glcdSetXY(canvas.width, 118);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdSetFont(&fontterminusmod32x2);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 3;     // Step in pixels between spectrum columns
    const uint8_t oft = 1;      // Offset of spectrum column inside step

    const uint8_t width = 2;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminusmod22x2;

    canvasShowTuner(dt, fmFont);
}

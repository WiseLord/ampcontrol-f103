#include "canvas.h"

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 480,
    .height = 320,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig120,
    .time.dmyFont = &fontterminusdig96,
    .time.wdFont = &fontterminusmod32x3,
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
    .menu.itemCnt = 10,
};

void gc480x320Init(Canvas **value)
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
    glcdSetFont(&fontterminusmod32x3);
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
    const tFont *fmFont = &fontterminusmod32x3;

    canvasShowTuner(dt, fmFont);
}

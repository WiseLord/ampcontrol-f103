#include "canvas.h"

static void showTime(bool clear, RTC_type *rtc);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 160,
    .height = 128,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig40,
    .time.dmyFont = &fontterminusdig30,
    .time.wdFont = &fontterminus24b,
    .time.hmsY = 8,
    .time.dmyY = 60,
    .time.wdY = 96,

    .bar.sc = 80,
    .bar.sw = 1,
    .bar.pos = 32,
    .bar.half = 6,
    .bar.middle = 2,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,
};

void gc160x128Init(Canvas **value)
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
    glcdSetFont(&fontterminus28);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max);

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

    canvasShowTuner(dt, fmFont);
}

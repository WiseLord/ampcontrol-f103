#include "canvas.h"

static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 128,
    .height = 64,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    .showTuner = showTuner,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig30,
    .time.dmyFont = &fontterminusdig22,
    .time.wdFont = &fontterminus12,
    .time.hmsY = 0,
    .time.dmyY = 30,
    .time.wdY = 52,

    .bar.sc = 64,
    .bar.sw = 1,
    .bar.pos = 26,
    .bar.half = 4,
    .bar.middle = 2,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 3,
};

void gm128x64Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = canvas.par->menu.itemCnt;
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(0, 0);
    glcdWriteString((char *)dp->label);

    canvasDrawBar(dp->value, dp->min, dp->max);

    glcdSetXY(canvas.width, 30);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdSetFont(&fontterminus12);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);

    glcdSetXY(104, 2);
//    glcdWriteIcon(dp->icon, icons_24, LCD_COLOR_WHITE, canvas.color);
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

    canvasShowTuner(dt, fmFont);
}

#include "canvas.h"

static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 480,
    .height = 320,
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

    .tune.lblFont = &fontterminusmod32x3,
    .tune.valFont = &fontterminusdig96,
    .tune.valY = 140,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_AQUA,

    .sp.step = 5,
    .sp.oft = 1,
    .sp.width = 3,
};

void gc480x320Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = canvas.par->menu.itemCnt;
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminusmod32x3;

    canvasShowTuner(dt, fmFont);
}

#include "canvas.h"

static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 176,
    .height = 132,
    .showTuner = showTuner,

    .par = &canvasParam,
};

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig42,
    .time.dmyFont = &fontterminusdig30,
    .time.wdFont = &fontterminus28,
    .time.hmsY = 8,
    .time.dmyY = 58,
    .time.wdY = 96,

    .bar.sc = 88,
    .bar.sw = 1,
    .bar.pos = 36,
    .bar.half = 6,
    .bar.middle = 2,

    .menu.headFont = &fontterminus16b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.lblFont = &fontterminus24b,
    .tune.valFont = &fontterminusdig40,
    .tune.valY = 80,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_AQUA,

    .sp.step = 2,
    .sp.oft = 0,
    .sp.width = 1,
};

void gc176x132Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = canvas.par->menu.itemCnt;
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminus32;

    canvasShowTuner(dt, fmFont);
}

#include "canvas.h"

static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 220,
    .height = 176,
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

    .tune.lblFont = &fontterminus32,
    .tune.valFont = &fontterminusmod32x2,
    .tune.valY = 118,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_AQUA,

    .sp.step = 3,
    .sp.oft = 1,
    .sp.width = 2,
};

void gc220x176Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = canvas.par->menu.itemCnt;
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminusmod22x2;

    canvasShowTuner(dt, fmFont);
}

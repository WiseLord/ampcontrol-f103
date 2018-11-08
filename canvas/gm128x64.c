#include "canvas.h"

static void showTuner(DispTuner *dt);

static const CanvasParam canvasParam;

static Canvas canvas = {
    .width = 128,
    .height = 64,
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

    .tune.lblFont = &fontterminus24b,
    .tune.valFont = &fontterminus12,
    .tune.valY = 30,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_WHITE,
};

void gm128x64Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = canvas.par->menu.itemCnt;
}

static void showTuner(DispTuner *dt)
{
    const tFont *fmFont = &fontterminus24;

    canvasShowTuner(dt, fmFont);
}

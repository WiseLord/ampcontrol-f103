#include "canvas.h"

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig120,
    .time.dmyFont = &fontterminusdig96,
    .time.wdFont = &fontterminusmod24x3,
    .time.hmsY = 4,
    .time.dmyY = 124,
    .time.wdY = 224,

    .bar.sc = 76,
    .bar.sw = 3,
    .bar.barY = 96,
    .bar.barW = 380,
    .bar.half = 15,
    .bar.middle = 2,

    .menu.headFont = &fontterminus32b,
    .menu.menuFont = &fontterminus24b,
    .menu.itemCnt = 10,

    .tune.lblFont = &fontterminusmod24x3,
    .tune.valFont = &fontterminusmod24x3,
    .tune.valY = 76,
    .tune.iconSet = &iconsamp64,
    .tune.iconColor = LCD_COLOR_AQUA,

    .sp.step = 5,
    .sp.oft = 1,
    .sp.width = 3,

    .tuner.lblFont = &fontterminusmod32x3,
};

static Canvas canvas = {
    .width = 480,
    .height = 320,

    .par = &canvasParam,
};

void gc480x320Init(Canvas **value)
{
    *value = &canvas;
}

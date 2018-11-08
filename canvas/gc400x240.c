#include "canvas.h"

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod32x2,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .bar.sc = 80,
    .bar.sw = 3,
    .bar.pos = 84,
    .bar.half = 14,
    .bar.middle = 2,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.lblFont = &fontterminusmod24x2,
    .tune.valFont = &fontterminusmod24x2,
    .tune.valY = 114,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_AQUA,

    .sp.step = 4,
    .sp.oft = 1,
    .sp.width = 2,

    .tuner.lblFont = &fontterminusmod24x2,
};

static Canvas canvas = {
    .width = 400,
    .height = 240,

    .par = &canvasParam,
};

void gc400x240Init(Canvas **value)
{
    *value = &canvas;
}

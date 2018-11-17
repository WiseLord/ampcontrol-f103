#include "canvas.h"

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod32x2,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .bar.sc = 60,
    .bar.sw = 2,
    .bar.barY = 64,
    .bar.barW = 240,
    .bar.half = 11,
    .bar.middle = 3,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.lblFont = &fontterminusmod18x3,
    .tune.valFont = &fontterminusmod18x3,
    .tune.valY = 48,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_WHITE,

    .sp.step = 3,
    .sp.oft = 0,
    .sp.width = 2,

    .tuner.lblFont = &fontterminusmod18x3,
};

static Canvas canvas = {
    .width = 320,
    .height = 240,

    .par = &canvasParam,
};

void gc320x240Init(Canvas **value)
{
    *value = &canvas;
}

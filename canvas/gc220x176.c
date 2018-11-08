#include "canvas.h"

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

    .tuner.lblFont = &fontterminus32,
};

static Canvas canvas = {
    .width = 220,
    .height = 176,

    .par = &canvasParam,
};

void gc220x176Init(Canvas **value)
{
    *value = &canvas;
}

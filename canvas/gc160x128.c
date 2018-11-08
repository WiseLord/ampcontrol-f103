#include "canvas.h"

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

    .tune.lblFont = &fontterminus28,
    .tune.valFont = &fontterminusdig40,
    .tune.valY = 88,
    .tune.iconSet = &iconsamp48,
    .tune.iconColor = LCD_COLOR_AQUA,

    .sp.step = 2,
    .sp.oft = 0,
    .sp.width = 1,

    .tuner.lblFont = &fontterminus28,
};

static Canvas canvas = {
    .width = 160,
    .height = 128,

    .par = &canvasParam,
};

void gc160x128Init(Canvas **value)
{
    *value = &canvas;
}

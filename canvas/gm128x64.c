#include "canvas.h"

static const CanvasParam canvasParam = {
    .time.hmsFont = &fontterminusdig30,
    .time.dmyFont = &fontterminusdig22,
    .time.wdFont = &fontterminus12,
    .time.hmsY = 0,
    .time.dmyY = 30,
    .time.wdY = 52,

    .bar.sc = 64,
    .bar.sw = 1,
    .bar.barY = 26,
    .bar.barW = 128,
    .bar.half = 4,
    .bar.middle = 2,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontamp08,
    .menu.itemCnt = 4,

    .tune.lblFont = &fontterminus24b,
    .tune.valFont = &fontterminus12,
    .tune.valY = 30,
    .tune.iconSet = &iconsamp24,
    .tune.iconColor = LCD_COLOR_WHITE,

    .sp.step = 2,
    .sp.oft = 1,
    .sp.width = 1,

    .tuner.lblFont = &fontterminus24b,
};

static Canvas canvas = {
    .width = 128,
    .height = 64,

    .par = &canvasParam,
};

void gm128x64Init(Canvas **value)
{
    *value = &canvas;
}

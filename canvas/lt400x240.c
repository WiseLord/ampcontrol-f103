#include "layout.h"

static const Layout lt400x240 = {
    .width = 400,
    .height = 240,

    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod32x2,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .tune.bar.sc = 80,
    .tune.bar.sw = 3,
    .tune.bar.barY = 84,
    .tune.bar.barW = 400,
    .tune.bar.half = 14,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.valFont = &fontterminusmod24x2,
    .tune.valY = 114,

    .sp.step = 4,
    .sp.oft = 1,
    .sp.width = 2,
    .sp.wfH = 2,

    .tuner.bar.sc = 80,
    .tuner.bar.sw = 3,
    .tuner.bar.barY = 84,
    .tuner.bar.barW = 400,
    .tuner.bar.half = 14,
    .tuner.bar.middle = 2,

    .iconSet = &iconsamp48,
    .iconColor = LCD_COLOR_WHITE,
    .lblFont = &fontterminusmod24x2,
};

const Layout *lt400x240Get(void)
{
    return &lt400x240;
}

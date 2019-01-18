#include "layout.h"

static const Layout lt320x240 = {
    .width = 320,
    .height = 240,

    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod18x3,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .tune.bar.sc = 60,
    .tune.bar.sw = 2,
    .tune.bar.barY = 72,
    .tune.bar.barW = 240,
    .tune.bar.half = 11,
    .tune.bar.middle = 3,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.valFont = &fontterminusmod18x3,
    .tune.valY = 56,

    .sp.step = 3,
    .sp.oft = 0,
    .sp.width = 2,
    .sp.wfH = 2,

    .tuner.bar.sc = 60,
    .tuner.bar.sw = 2,
    .tuner.bar.barY = 72,
    .tuner.bar.barW = 240,
    .tuner.bar.half = 11,
    .tuner.bar.middle = 3,

    .iconSet = &iconsamp48,
    .iconColor = LCD_COLOR_WHITE,
    .lblFont = &fontterminusmod18x3,
};

const Layout *lt320x240Get(void)
{
    return &lt320x240;
}

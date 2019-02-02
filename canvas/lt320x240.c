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

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.valFont = &fontterminusmod18x3,
    .tune.valY = 56,
    .tune.bar.sc = 60,
    .tune.bar.sw = 2,
    .tune.bar.barY = 60,
    .tune.bar.barW = 240,
    .tune.bar.half = 10,
    .tune.bar.middle = 2,

    .sp.step = 3,
    .sp.oft = 0,
    .sp.width = 2,
    .sp.wfH = 2,

    .tuner.stFont = &fontterminusmod18x3,
    .tuner.bar.sc = 66,
    .tuner.bar.sw = 2,
    .tuner.bar.barY = 60,
    .tuner.bar.barW = 264,
    .tuner.bar.half = 10,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 3,

    .iconSet = &iconsamp48,
    .lblFont = &fontterminusmod18x3,
};

const Layout *lt320x240Get(void)
{
    return &lt320x240;
}

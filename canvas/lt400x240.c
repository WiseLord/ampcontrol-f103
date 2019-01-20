#include "layout.h"

static const Layout lt400x240 = {
    .width = 400,
    .height = 240,

    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod18x3,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .tune.bar.sc = 80,
    .tune.bar.sw = 2,
    .tune.bar.barY = 60,
    .tune.bar.barW = 320,
    .tune.bar.half = 10,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.valFont = &fontterminusmod18x3,
    .tune.valY = 56,

    .sp.step = 4,
    .sp.oft = 1,
    .sp.width = 2,
    .sp.wfH = 2,

    .tuner.bar.sc = 100,
    .tuner.bar.sw = 2,
    .tuner.bar.barY = 60,
    .tuner.bar.barW = 400,
    .tuner.bar.half = 10,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 3,

    .iconSet = &iconsamp48,
    .lblFont = &fontterminusmod18x3,
};

const Layout *lt400x240Get(void)
{
    return &lt400x240;
}

#include "layout.h"

static const Layout lt220x176 = {
    .width = 220,
    .height = 176,

    .time.hmsFont = &fontterminusdig58,
    .time.dmyFont = &fontterminusdig40,
    .time.wdFont = &fontterminus32,
    .time.hmsY = 10,
    .time.dmyY = 78,
    .time.wdY = 126,

    .tune.bar.sc = 73,
    .tune.bar.sw = 2,
    .tune.bar.barY = 54,
    .tune.bar.barW = 220,
    .tune.bar.half = 10,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus20b,
    .menu.menuFont = &fontterminus18,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminusmod32x2,
    .tune.valY = 118,

    .sp.step = 3,
    .sp.oft = 1,
    .sp.width = 2,
    .sp.wfH = 2,

    .tuner.bar.sc = 73,
    .tuner.bar.sw = 2,
    .tuner.bar.barY = 54,
    .tuner.bar.barW = 220,
    .tuner.bar.half = 10,
    .tuner.bar.middle = 2,

    .iconSet = &iconsamp32,
    .lblFont = &fontterminus32,
};

const Layout *lt220x176Get(void)
{
    return &lt220x176;
}

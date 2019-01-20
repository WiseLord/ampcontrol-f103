#include "layout.h"

static const Layout lt480x320 = {
    .width = 480,
    .height = 320,

    .time.hmsFont = &fontterminusdig120,
    .time.dmyFont = &fontterminusdig96,
    .time.wdFont = &fontterminusmod24x3,
    .time.hmsY = 4,
    .time.dmyY = 124,
    .time.wdY = 224,

    .tune.bar.sc = 76,
    .tune.bar.sw = 3,
    .tune.bar.barY = 80,
    .tune.bar.barW = 380,
    .tune.bar.half = 15,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus32b,
    .menu.menuFont = &fontterminus24b,
    .menu.itemCnt = 10,

    .tune.valFont = &fontterminusmod24x3,
    .tune.valY = 70,

    .sp.step = 5,
    .sp.oft = 1,
    .sp.width = 3,
    .sp.wfH = 3,

    .tuner.bar.sc = 96,
    .tuner.bar.sw = 3,
    .tuner.bar.barY = 80,
    .tuner.bar.barW = 480,
    .tuner.bar.half = 15,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 4,

    .iconSet = &iconsamp64,
    .lblFont = &fontterminusmod24x3,
};

const Layout *lt480x320Get(void)
{
    return &lt480x320;
}

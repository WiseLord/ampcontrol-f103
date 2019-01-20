#include "layout.h"

static const Layout lt160x128 = {
    .width = 160,
    .height = 128,

    .time.hmsFont = &fontterminusdig40,
    .time.dmyFont = &fontterminusdig30,
    .time.wdFont = &fontterminus28b,
    .time.hmsY = 4,
    .time.dmyY = 54,
    .time.wdY = 96,

    .tune.bar.sc = 60,
    .tune.bar.sw = 1,
    .tune.bar.barY = 30,
    .tune.bar.barW = 120,
    .tune.bar.half = 5,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminus28b,
    .tune.valY = 28,

    .sp.step = 2,
    .sp.oft = 0,
    .sp.width = 1,
    .sp.wfH = 1,

    .tuner.bar.sc = 80,
    .tuner.bar.sw = 1,
    .tuner.bar.barY = 30,
    .tuner.bar.barW = 160,
    .tuner.bar.half = 5,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 1,

    .iconSet = &iconsamp24,
    .lblFont = &fontterminus28b,
};

const Layout *lt160x128Get(void)
{
    return &lt160x128;
}

#include "layout.h"

static const Layout lt160x128 = {
    .width = 160,
    .height = 128,

    .time.hmsFont = &fontterminusdig40,
    .time.dmyFont = &fontterminusdig30,
    .time.wdFont = &fontterminus24b,
    .time.hmsY = 8,
    .time.dmyY = 60,
    .time.wdY = 96,

    .tune.bar.sc = 80,
    .tune.bar.sw = 1,
    .tune.bar.barY = 32,
    .tune.bar.barW = 160,
    .tune.bar.half = 6,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminusdig40,
    .tune.valY = 88,

    .sp.step = 2,
    .sp.oft = 0,
    .sp.width = 1,
    .sp.wfH = 1,

    .tuner.bar.sc = 80,
    .tuner.bar.sw = 1,
    .tuner.bar.barY = 32,
    .tuner.bar.barW = 160,
    .tuner.bar.half = 6,
    .tuner.bar.middle = 2,

    .iconSet = &iconsamp24,
    .lblFont = &fontterminus28,
};

const Layout *lt160x128Get(void)
{
    return &lt160x128;
}

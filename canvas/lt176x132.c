#include "layout.h"

static const Layout lt176x132 = {
    .width = 176,
    .height = 132,

    .time.hmsFont = &fontterminusdig42,
    .time.dmyFont = &fontterminusdig30,
    .time.wdFont = &fontterminus28,
    .time.hmsY = 8,
    .time.dmyY = 58,
    .time.wdY = 96,

    .tune.bar.sc = 88,
    .tune.bar.sw = 1,
    .tune.bar.barY = 36,
    .tune.bar.barW = 176,
    .tune.bar.half = 6,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus16b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminusdig40,
    .tune.valY = 80,

    .sp.step = 2,
    .sp.oft = 0,
    .sp.width = 1,
    .sp.wfH = 1,

    .tuner.bar.sc = 88,
    .tuner.bar.sw = 1,
    .tuner.bar.barY = 36,
    .tuner.bar.barW = 176,
    .tuner.bar.half = 6,
    .tuner.bar.middle = 2,

    .iconSet = &iconsamp24,
    .iconColor = LCD_COLOR_WHITE,
    .lblFont = &fontterminus24b,
};

const Layout *lt176x132Get(void)
{
    return &lt176x132;
}

#include "canvas.h"

static const Layout lt128x64 = {
    .width = 128,
    .height = 64,

    .time.hmsFont = &fontterminusdig30,
    .time.dmyFont = &fontterminusdig22,
    .time.wdFont = &fontterminus12,
    .time.hmsY = 0,
    .time.dmyY = 30,
    .time.wdY = 52,

    .tune.bar.sc = 64,
    .tune.bar.sw = 1,
    .tune.bar.barY = 26,
    .tune.bar.barW = 128,
    .tune.bar.half = 4,
    .tune.bar.middle = 2,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontamp08,
    .menu.itemCnt = 4,

    .tune.valFont = &fontterminus12,
    .tune.valY = 30,

    .sp.step = 2,
    .sp.oft = 1,
    .sp.width = 1,
    .sp.wfH = 1,

    .tuner.bar.sc = 64,
    .tuner.bar.sw = 1,
    .tuner.bar.barY = 26,
    .tuner.bar.barW = 128,
    .tuner.bar.half = 4,
    .tuner.bar.middle = 2,

    .iconSet = &iconsamp24,
    .iconColor = LCD_COLOR_WHITE,
    .lblFont = &fontterminus24b,
};

void lt128x64Init(Canvas *canvas)
{
    canvas->lt = &lt128x64;
}

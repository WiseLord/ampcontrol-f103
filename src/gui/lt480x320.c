#include "layout.h"

static const Layout lt480x320 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 480,
    .rect.h = 320,

    .time.hmsFont = &fontterminusdig120,
    .time.dmyFont = &fontterminusdig96,
    .time.wdFont = &fontterminusmod24x3,
    .time.hmsY = 4,
    .time.dmyY = 124,
    .time.wdY = 224,

    .menu.headFont = &fontterminus32b,
    .menu.menuFont = &fontterminus24b,
    .menu.itemCnt = 10,

    .tune.labelFont = &fontterminusmod24x3,
    .tune.valFont = &fontterminusmod24x3,
    .tune.valY = 70,
    .tune.bar.sc = 76,
    .tune.bar.sw = 3,
    .tune.bar.barY = 80,
    .tune.bar.barW = 380,
    .tune.bar.half = 15,
    .tune.bar.middle = 2,

    .tuner.stFont = &fontterminusmod24x3,
    .tuner.nameFont = &fontterminus32b,
    .tuner.bar.sc = 82,
    .tuner.bar.sw = 3,
    .tuner.bar.barY = 80,
    .tuner.bar.barW = 410,
    .tuner.bar.half = 15,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 4,

    .rds.psFont = &fontterminus32b,
    .rds.textFont = &fontterminus24b,

    .textEdit.rect.x = 40,
    .textEdit.rect.y = 80,
    .textEdit.rect.w = 400,
    .textEdit.rect.h = 160,
    .textEdit.editFont = &fontterminus32b,

    .iconSet = &iconsamp64,
    .lblFont = &fontterminusmod24x3,
};

const Layout *layoutGet(void)
{
    return &lt480x320;
}

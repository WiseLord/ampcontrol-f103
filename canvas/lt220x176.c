#include "layout.h"

static const Layout lt220x176 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 220,
    .rect.h = 176,

    .time.hmsFont = &fontterminusdig58,
    .time.dmyFont = &fontterminusdig40,
    .time.wdFont = &fontterminus32,
    .time.hmsY = 10,
    .time.dmyY = 78,
    .time.wdY = 126,

    .menu.headFont = &fontterminus20b,
    .menu.menuFont = &fontterminus18,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminus32,
    .tune.valY = 36,
    .tune.bar.sc = 58,
    .tune.bar.sw = 2,
    .tune.bar.barY = 40,
    .tune.bar.barW = 174,
    .tune.bar.half = 7,
    .tune.bar.middle = 2,

    .sp.step = 3,
    .sp.oft = 1,
    .sp.width = 2,
    .sp.wfH = 2,

    .tuner.stFont = &fontterminus32,
    .tuner.bar.sc = 62,
    .tuner.bar.sw = 2,
    .tuner.bar.barY = 40,
    .tuner.bar.barW = 186,
    .tuner.bar.half = 7,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 2,

    .textEdit.rect.x = 30,
    .textEdit.rect.y = 28,
    .textEdit.rect.w = 160,
    .textEdit.rect.h = 120,
    .textEdit.editFont = &fontterminus20b,

    .iconSet = &iconsamp32,
    .lblFont = &fontterminus32,
};

const Layout *lt220x176Get(void)
{
    return &lt220x176;
}

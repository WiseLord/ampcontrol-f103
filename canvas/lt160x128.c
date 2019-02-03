#include "layout.h"

static const Layout lt160x128 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 160,
    .rect.h = 128,

    .time.hmsFont = &fontterminusdig40,
    .time.dmyFont = &fontterminusdig30,
    .time.wdFont = &fontterminus28b,
    .time.hmsY = 4,
    .time.dmyY = 54,
    .time.wdY = 96,

    .menu.headFont = &fontterminus14b,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminus28b,
    .tune.valY = 28,
    .tune.bar.sc = 58,
    .tune.bar.sw = 1,
    .tune.bar.barY = 30,
    .tune.bar.barW = 116,
    .tune.bar.half = 5,
    .tune.bar.middle = 2,

    .sp.step = 2,
    .sp.oft = 0,
    .sp.width = 1,
    .sp.wfH = 1,

    .tuner.stFont = &fontterminus28b,
    .tuner.bar.sc = 64,
    .tuner.bar.sw = 1,
    .tuner.bar.barY = 30,
    .tuner.bar.barW = 128,
    .tuner.bar.half = 5,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 1,

    .textEdit.rect.x = 20,
    .textEdit.rect.y = 24,
    .textEdit.rect.w = 120,
    .textEdit.rect.h = 80,
    .textEdit.editFont = &fontterminus14b,

    .iconSet = &iconsamp24,
    .lblFont = &fontterminus28b,
};

const Layout *lt160x128Get(void)
{
    return &lt160x128;
}

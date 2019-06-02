#include "layout.h"

static const Layout lt320x240 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 320,
    .rect.h = 240,

    .time.hmsFont = &fontterminusdig80,
    .time.dmyFont = &fontterminusdig64,
    .time.wdFont = &fontterminusmod18x3,
    .time.hmsY = 10,
    .time.dmyY = 100,
    .time.wdY = 170,

    .menu.headFont = &fontterminus28b,
    .menu.menuFont = &fontterminus22b,
    .menu.itemCnt = 8,

    .tune.valFont = &fontterminusmod18x3,
    .tune.valY = 56,
    .tune.bar.sc = 60,
    .tune.bar.sw = 2,
    .tune.bar.barY = 60,
    .tune.bar.barW = 240,
    .tune.bar.half = 10,
    .tune.bar.middle = 2,

    .tuner.stFont = &fontterminusmod18x3,
    .tuner.nameFont = &fontterminus28b,
    .tuner.bar.sc = 66,
    .tuner.bar.sw = 2,
    .tuner.bar.barY = 60,
    .tuner.bar.barW = 264,
    .tuner.bar.half = 10,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 3,

    .rds.psFont = &fontterminus28b,
    .rds.textFont = &fontterminus22b,

    .textEdit.rect.x = 30,
    .textEdit.rect.y = 50,
    .textEdit.rect.w = 260,
    .textEdit.rect.h = 140,
    .textEdit.editFont = &fontterminus28b,

    .iconSet = &iconsamp48,
    .lblFont = &fontterminusmod18x3,
};

const Layout *layoutGet(void)
{
    return &lt320x240;
}

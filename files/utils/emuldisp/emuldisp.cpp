#include "emuldisp.h"

#include "../../../display/dispdrv.h"
#include "../../../tr/labels.h"
#include "../../../menu.h"
#include "../../../screen.h"

#define RGB(x) QColor(QRgb( ((x & 0xF800) << 8) | ((x & 0xE000) << 3) | \
                            ((x & 0x07E0) << 5) | ((x & 0x0600) >> 1) | \
                            ((x & 0x001F) << 3) | ((x & 0x001C) >> 3) ))


EmulDisp::EmulDisp(QWidget *parent) :
    QWidget(parent)
{
    painter = new QPainter;

    labelsInit();
    glcdInit(&this->glcd);
    canvasInit(&this->canvas);
    canvas->glcd = this->glcd;

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(canvas->color);
}

void EmulDisp::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    QPen pen(RGB(color));

    painter->begin(this);
    painter->setPen(pen);
#if EMUL_DISP_SCALE == 3
    painter->drawPoint(3 * x + 1, 3 * y + 1);
    painter->drawPoint(3 * x + 1, 3 * y + 2);
    painter->drawPoint(3 * x + 2, 3 * y + 1);
    painter->drawPoint(3 * x + 2, 3 * y + 2);
#elif EMUL_DISP_SCALE == 2
    painter->drawPoint(2 * x + 0, 2 * y + 0);
    painter->drawPoint(2 * x + 0, 2 * y + 1);
    painter->drawPoint(2 * x + 1, 2 * y + 0);
    painter->drawPoint(2 * x + 1, 2 * y + 1);
#else
    painter->drawPoint(x, y);
#endif
    painter->end();
}

void EmulDisp::setSize(uint16_t w, uint16_t h)
{
#if EMUL_DISP_SCALE == 3
    this->resize(3 * w + 1, 3 * h + 1);
#elif EMUL_DISP_SCALE == 2
    this->resize(2 * w + 0, 2 * h + 0);
#else
    this->resize(w, h);
#endif
}

void EmulDisp::paintEvent(QPaintEvent *pe)
{
    (void)pe;

    drawScreen();
}

void EmulDisp::drawScreen()
{
    painter->begin(this);
    painter->fillRect(0, 0, this->width(), this->height(), QColor(QRgb(0x101010)));
    painter->end();

    glcdDrawRect(0, 0, this->width() & 0xFFFF, this->height() & 0xFFFF, LCD_COLOR_BLACK);

    screenShow();
}

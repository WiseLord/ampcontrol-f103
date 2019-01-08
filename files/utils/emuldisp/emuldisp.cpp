#include "emuldisp.h"

#include "../../../display/dispdrv.h"
#include "../../../tr/labels.h"
#include "../../../menu.h"
#include "../../../screen.h"

#include "../../../display/glcd.h"
#include "../../../canvas/canvas.h"

#include "emulscreen.h"

#define RGB(x) QColor(QRgb( ((x & 0xF800) << 8) | ((x & 0xE000) << 3) | \
                            ((x & 0x07E0) << 5) | ((x & 0x0600) >> 1) | \
                            ((x & 0x001F) << 3) | ((x & 0x001C) >> 3) ))


EmulDisp::EmulDisp(QWidget *parent) :
    QWidget(parent)
{
    painter = new QPainter;

    labelsInit();
    canvasInit();
}

void EmulDisp::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    const QColor qcolor = RGB(color);
    QPen pen(qcolor);

    painter->begin(this);
    painter->setPen(pen);
#if EMUL_DISP_SCALE >=3
    painter->fillRect(EMUL_DISP_SCALE * x, EMUL_DISP_SCALE * y, EMUL_DISP_SCALE - 1, EMUL_DISP_SCALE - 1, qcolor);
#elif EMUL_DISP_SCALE == 2
    painter->fillRect(EMUL_DISP_SCALE * x, EMUL_DISP_SCALE * y, EMUL_DISP_SCALE, EMUL_DISP_SCALE, qcolor);
#else
    painter->drawPoint(x, y);
#endif
    painter->end();
}

void EmulDisp::setSize(uint16_t w, uint16_t h)
{
#if EMUL_DISP_SCALE >= 3
    this->resize(EMUL_DISP_SCALE * w + 1, EMUL_DISP_SCALE * h + 1);
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
    painter->fillRect(0, 0, this->width(), this->height(), QColor(64, 64, 64));
    painter->end();

    glcdDrawRect(0, 0, static_cast<int16_t>((this->width() & 0xFFFF)), static_cast<int16_t>(this->height() & 0xFFFF), LCD_COLOR_BLACK);

    screenShow();
}

#include "emuldisp.h"
#include <QApplication>

#include "../../../display/dispdrv.h"

#define EMUL_DISP_WIDTH     320
#define EMUL_DISP_HEIGHT    240
#define EMUL_DISP_SCALE     2

#define RGB(x) QColor(QRgb( ((x & 0xF800) << 8) | ((x & 0xE000) << 3) | \
                            ((x & 0x07E0) << 5) | ((x & 0x0600) >> 1) | \
                            ((x & 0x001F) << 3) | ((x & 0x001C) >> 3) ))

extern "C" void emulDrawPixel(int16_t x, int16_t y, uint16_t color);
static DispDriver drv;

void dispdrvInit(DispDriver **driver)
{
    drv.width = EMUL_DISP_WIDTH;
    drv.height = EMUL_DISP_HEIGHT;
    drv.drawPixel = emulDrawPixel;
    drv.drawImage = glcdDrawImage;

    *driver = &drv;
}

EmulDisp::EmulDisp(QWidget *parent) :
    QWidget(parent)
{
    painter = new QPainter;

    glcdInit(&this->glcd);
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

    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdSetFontBgColor(LCD_COLOR_BLACK);

    glcdSetFont(&fontterminusdig120);

    glcdSetXY(1, 1);
    glcdWriteString("134");
}

static EmulDisp *disp;

void emulDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    disp->drawPixel(x, y, color);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EmulDisp emulDisp;
    emulDisp.setSize(EMUL_DISP_WIDTH, EMUL_DISP_HEIGHT);
    emulDisp.show();

    disp = &emulDisp;

    return a.exec();
}

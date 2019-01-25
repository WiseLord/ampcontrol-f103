#include "emuldisp.h"

#include "../../../canvas/layout.h"
#include "../../../display/dispdefs.h"
#include "../../../input.h"
#include "../../../screen.h"
#include "../../../swtimers.h"
#include "../../../tr/labels.h"

#include <QDebug>

#define RGB(x) QColor(QRgb( ((x & 0xF800) << 8) | ((x & 0xE000) << 3) | \
                            ((x & 0x07E0) << 5) | ((x & 0x0600) >> 1) | \
                            ((x & 0x001F) << 3) | ((x & 0x001C) >> 3) ))


EmulDisp::EmulDisp(QWidget *parent) :
    QWidget(parent)
{
    action = {ACTION_STANDBY, FLAG_ON};
    brightness = LCD_BR_MAX;

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), SLOT(drawscreen()));

    mSecTimer = new QTimer(this);
    connect(mSecTimer, SIGNAL(timeout()), SLOT(systick()));
}

void EmulDisp::init()
{
    screenInit();
    inputInit();

    audioReadSettings();
    tunerReadSettings();

    updateTimer->start(40);
    mSecTimer->start(1);
}

void EmulDisp::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    QColor qcolor = RGB(color);
    qcolor = qcolor.lighter(100 * (brightness + 3) / (LCD_BR_MAX + 3));

    QPainter painter(&image);
    QRect rect(EMUL_DISP_SCALE * (x + EMUL_DISP_BORDER),
               EMUL_DISP_SCALE * (y + EMUL_DISP_BORDER),
               EMUL_DISP_SCALE,
               EMUL_DISP_SCALE);

    painter.fillRect(rect, qcolor);
    update(rect);
}

void EmulDisp::drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    QColor qcolor = RGB(color);
    qcolor = qcolor.lighter(100 * (brightness + 3) / (LCD_BR_MAX + 3));

    QPainter painter(&image);
    QRect rect(EMUL_DISP_SCALE * (x + EMUL_DISP_BORDER),
               EMUL_DISP_SCALE * (y + EMUL_DISP_BORDER),
               w * EMUL_DISP_SCALE,
               h * EMUL_DISP_SCALE);

    painter.fillRect(rect, qcolor);
    update(rect);
}

void EmulDisp::drawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    QColor qcolor = RGB(color);
    qcolor = qcolor.lighter(100 * (brightness + 3) / (LCD_BR_MAX + 3));

    QColor qBgcolor = RGB(bgColor);
    qBgcolor = qBgcolor.lighter(100 * (brightness + 3) / (LCD_BR_MAX + 3));

    QRect rect(EMUL_DISP_SCALE * (x + EMUL_DISP_BORDER),
               EMUL_DISP_SCALE * (y + EMUL_DISP_BORDER),
               img->width * EMUL_DISP_SCALE,
               img->height * EMUL_DISP_SCALE);

    QPainter painter(&image);

    int16_t w = img->width;
    int16_t h = img->height;
    for (uint16_t j = 0; j < (h + 7) / 8; j++) {
        for (uint16_t i = 0; i < w; i++) {
            uint8_t data = img->data[w * j + i];
            for (uint8_t bit = 0; bit < 8; bit++) {
                if (8 * j + bit < h) {
                    QRect pixRect(EMUL_DISP_SCALE * (x + i + EMUL_DISP_BORDER),
                                  EMUL_DISP_SCALE * (y + (8 * j + bit) + EMUL_DISP_BORDER),
                                  EMUL_DISP_SCALE,
                                  EMUL_DISP_SCALE);
                    painter.fillRect(pixRect, data & (1 << bit) ? qcolor : qBgcolor);
                }
            }
        }
    }

    update(rect);
}

void EmulDisp::setSize(uint16_t w, uint16_t h)
{
    this->resize(EMUL_DISP_SCALE * w, EMUL_DISP_SCALE * h);
}

void EmulDisp::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QRect dirtyRect = pe->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void EmulDisp::resizeEvent(QResizeEvent *event)
{
    resizeImage(&image, QSize(width(), height()));
    QWidget::resizeEvent(event);
}

void EmulDisp::systick()
{
    inputPoll();
    swTimUpdate();
}

void EmulDisp::drawscreen()
{
    actionHandle(action, ACTION_VISIBLE);
    screenShow(false);
    action = actionUserGet();
}

void EmulDisp::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Qt::black);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void EmulDisp::setBrightness(const int8_t &value)
{
    brightness = value;
}

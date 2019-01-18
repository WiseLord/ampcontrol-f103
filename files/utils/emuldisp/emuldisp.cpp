#include "emuldisp.h"

#include "../../../display/dispdrv.h"
#include "../../../tr/labels.h"
#include "../../../menu.h"
#include "../../../screen.h"

#include "../../../display/glcd.h"
#include "../../../canvas/layout.h"

#include "emulscreen.h"
#include <QDebug>

#define RGB(x) QColor(QRgb( ((x & 0xF800) << 8) | ((x & 0xE000) << 3) | \
                            ((x & 0x07E0) << 5) | ((x & 0x0600) >> 1) | \
                            ((x & 0x001F) << 3) | ((x & 0x001C) >> 3) ))


EmulDisp::EmulDisp(QWidget *parent) :
    QWidget(parent)
{
    painter = new QPainter;

    cMenu = new QMenu(this);

    actScreenTime = new QAction("Show time", this);
    actScreenSpectrum = new QAction("Show spectrum", this);
    actScreenBrightness = new QAction("Show brightness", this);
    actScreenInput = new QAction("Show input", this);
    actScreenAudio = new QAction("Show audio", this);
    actScreenTuner = new QAction("Show tuner", this);
    actScreenMenu = new QAction("Show menu", this);

    cMenu->addAction(actScreenTime);
    cMenu->addAction(actScreenSpectrum);
    cMenu->addAction(actScreenBrightness);
    cMenu->addAction(actScreenInput);
    cMenu->addAction(actScreenAudio);
    cMenu->addAction(actScreenTuner);
    cMenu->addAction(actScreenMenu);

    connect(cMenu,
            SIGNAL(triggered(QAction *)),
            SLOT(menuSelected(QAction *))
           );

    labelsInit();
    layoutInit();

    screenSet(SCREEN_STANDBY);
}

void EmulDisp::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    const QColor qcolor = RGB(color);
    QPen pen(qcolor);

    painter->begin(this);
    painter->setPen(pen);
#if EMUL_DISP_SCALE >=3
    painter->fillRect(EMUL_DISP_SCALE * x, EMUL_DISP_SCALE * y, EMUL_DISP_SCALE - 1,
                      EMUL_DISP_SCALE - 1, qcolor);
#elif EMUL_DISP_SCALE == 2
    painter->fillRect(EMUL_DISP_SCALE * x, EMUL_DISP_SCALE * y, EMUL_DISP_SCALE, EMUL_DISP_SCALE,
                      qcolor);
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

    painter->begin(this);
    painter->fillRect(0, 0, this->width(), this->height(), QColor(64, 64, 64));
    painter->end();

    glcdDrawRect(0, 0, static_cast<int16_t>((this->width() & 0xFFFF)),
                 static_cast<int16_t>(this->height() & 0xFFFF), LCD_COLOR_BLACK);

    screenShow();
}

void EmulDisp::contextMenuEvent(QContextMenuEvent *pe)
{
    cMenu->exec(pe->globalPos());
}

void EmulDisp::menuSelected(QAction *action)
{
    qDebug() << "Selected action" << action->text();

    if (action == actScreenTime) {
        screenSet(SCREEN_TIME);
    } else if (action == actScreenSpectrum) {
        screenSet(SCREEN_SPECTRUM);
    } else if (action == actScreenBrightness) {
        screenSet(SCREEN_BRIGHTNESS);
    } else if (action == actScreenInput) {
        screenSet(SCREEN_AUDIO_INPUT);
    } else if (action == actScreenAudio) {
        screenSet(SCREEN_AUDIO_PARAM);
    } else if (action == actScreenTuner) {
        screenSet(SCREEN_TUNER);
    } else if (action == actScreenMenu) {
        screenSet(SCREEN_MENU);
    } else {
        screenSet(SCREEN_STANDBY);
    }

    update();
}

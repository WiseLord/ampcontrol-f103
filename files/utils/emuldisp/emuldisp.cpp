#include "emuldisp.h"

#include "../../../canvas/layout.h"
#include "../../../display/dispdrv.h"
#include "../../../display/glcd.h"
#include "../../../menu.h"
#include "../../../screen.h"
#include "../../../tr/labels.h"
#include "../../../tuner/tuner.h"

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

    tunerReadSettings();
    audioReadSettings();
    screenSet(EMUL_SCREEN_INIT);

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), SLOT(update()));
    updateTimer->start(1000);
}

void EmulDisp::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    const QColor qcolor = RGB(color);

    painter->begin(this);
    painter->fillRect(EMUL_DISP_SCALE * (x + EMUL_DISP_BORDER),
                      EMUL_DISP_SCALE * (y + EMUL_DISP_BORDER),
                      EMUL_DISP_SCALE,
                      EMUL_DISP_SCALE,
                      qcolor);
    painter->end();
}

void EmulDisp::drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    const QColor qcolor = RGB(color);

    painter->begin(this);
    painter->fillRect(EMUL_DISP_SCALE * (x + EMUL_DISP_BORDER),
                      EMUL_DISP_SCALE * (y + EMUL_DISP_BORDER),
                      w * EMUL_DISP_SCALE,
                      h * EMUL_DISP_SCALE,
                      qcolor);
    painter->end();
}

void EmulDisp::setSize(uint16_t w, uint16_t h)
{
    this->resize(EMUL_DISP_SCALE * w, EMUL_DISP_SCALE * h);
}

void EmulDisp::paintEvent(QPaintEvent *pe)
{
    (void)pe;

    if (screenGet() == SCREEN_MENU) {
        menuSetActive(MENU_TUNER_BASS);
        menuChange(+1);
        menuChange(+1);
        menuChange(+1);
        menuSetActive(MENU_TUNER_STEP);
    }
    screenShow(true);

    // Draw frame
    painter->begin(this);
    painter->setPen(QPen(Qt::black, EMUL_DISP_SCALE * EMUL_DISP_BORDER * 2));
    painter->drawRect(0, 0, this->width(), this->height());
    painter->end();
}

void EmulDisp::contextMenuEvent(QContextMenuEvent *pe)
{
    cMenu->exec(pe->globalPos());
}

void EmulDisp::menuSelected(QAction *action)
{
    ScreenParam scrPar;

    if (action == actScreenTime) {
        screenSet(SCREEN_TIME);
    } else if (action == actScreenSpectrum) {
        screenSet(SCREEN_SPECTRUM);
    } else if (action == actScreenBrightness) {
        screenSet(SCREEN_BRIGHTNESS);
    } else if (action == actScreenInput) {
        screenSet(SCREEN_AUDIO_INPUT);
    } else if (action == actScreenAudio) {
        scrPar.tune = AUDIO_TUNE_VOLUME;
        screenSetParam(scrPar);
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

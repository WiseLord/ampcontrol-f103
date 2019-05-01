#include "amp.h"

#include "emuldisp.h"
#include "emuldisp_stub.h"

#include <QDebug>

#include "../../../canvas/layout.h"
#include "../../../eemap.h"
#include "../../../input.h"

static EmulDisp *disp;
static Amp *amp;

extern "C" void eeUpdateRaw(uint16_t addr, uint16_t data)
{
    amp->eeUpdateRaw(addr, data);
}

extern "C" uint16_t eeReadRaw(uint16_t addr)
{
    return amp->eeReadRaw(addr);
}

extern "C" void dispdrvDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    disp->drawPixel(x, y, color);
}

extern "C" void dispdrvDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    disp->drawRectangle(x, y, w, h, color);
}

extern "C" void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    disp->drawImage(img, x, y, color, bgColor);
}

extern "C" void dispdrvInit(void)
{
}

extern "C" uint8_t dispdrvGetBus(void)
{
    return amp->bus;
}

extern "C" void dispdrvSetBrightness(int8_t value)
{
    disp->setBrightness(value);
}

extern "C" const Layout *ltEmulGet(void)
{
    const Layout *lt;

#if EMUL_DISP_WIDTH == 160 && EMUL_DISP_HEIGHT == 128
    lt = lt160x128Get();
#elif EMUL_DISP_WIDTH == 176 && EMUL_DISP_HEIGHT == 132
    lt = lt176x132Get();
#elif EMUL_DISP_WIDTH == 220 && EMUL_DISP_HEIGHT == 176
    lt = lt220x176Get();
#elif EMUL_DISP_WIDTH == 320 && EMUL_DISP_HEIGHT == 240
    lt = lt320x240Get();
#elif EMUL_DISP_WIDTH == 400 && EMUL_DISP_HEIGHT == 240
    lt = lt400x240Get();
#elif EMUL_DISP_WIDTH == 480 && EMUL_DISP_HEIGHT == 320
    lt = lt480x320Get();
#else
#error "No such layout"
#endif

    return lt;
}

Amp::Amp(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    settings = new QSettings(ORGANIZATION_NAME, APPLICATION_NAME);

    amp = this;
    disp = getEmulDisp();

    this->bus = BTN_NO;

    disp->setFixedSize((EMUL_DISP_WIDTH + 2 * EMUL_DISP_BORDER) * EMUL_DISP_SCALE,
                       (EMUL_DISP_HEIGHT + 2 * EMUL_DISP_BORDER) * EMUL_DISP_SCALE);
    setFixedSize(sizeHint());

    dialValue = dial->value();
    dialMax = dial->maximum();
}

void Amp::initDisp()
{
    disp->init();
}

EmulDisp *Amp::getEmulDisp()
{
    return emulDisp;
}

void Amp::eeUpdateRaw(uint16_t addr, uint16_t data)
{
    settings->setValue(QString::number((uint)addr, 16).rightJustified(4, '0'), data);
}

uint16_t Amp::eeReadRaw(uint16_t addr)
{
    if (settings->contains(QString::number(addr, 16).rightJustified(4, '0'))) {
        return (uint16_t)(settings->value(QString::number(addr, 16).rightJustified(4, '0')).toUInt());
    }

    return EE_NOT_FOUND;
}

void Amp::dialChanged(int value)
{
    int diff = (value + dialMax - dialValue) % dialMax;

    if (diff > dialMax / 2) {
        diff = -1;
    } else if (diff < dialMax / 2) {
        diff = 1;
    }

    actionQueue(ACTION_ENCODER, diff);

    dialValue = value;

    this->statusBar()->showMessage(QString("ENC value: %1").arg(diff), 200);
}

void Amp::on_btn0_pressed()
{
    this->bus |= BTN_D0;
    this->statusBar()->showMessage("BTN_D0 pressed", 1000);
}

void Amp::on_btn0_released()
{
    this->bus &= ~BTN_D0;
    this->statusBar()->showMessage("BTN_D0 released", 1000);
}

void Amp::on_btn1_pressed()
{
    this->bus |= BTN_D1;
    this->statusBar()->showMessage("BTN_D1 pressed", 1000);
}

void Amp::on_btn1_released()
{
    this->bus &= ~BTN_D1;
    this->statusBar()->showMessage("BTN_D1 released", 1000);
}

void Amp::on_btn2_pressed()
{
    this->bus |= BTN_D2;
    this->statusBar()->showMessage("BTN_D2 pressed", 1000);
}

void Amp::on_btn2_released()
{
    this->bus &= ~BTN_D2;
    this->statusBar()->showMessage("BTN_D2 released", 1000);
}

void Amp::on_btn3_pressed()
{
    this->bus |= BTN_D3;
    this->statusBar()->showMessage("BTN_D3 pressed", 1000);
}

void Amp::on_btn3_released()
{
    this->bus &= ~BTN_D3;
    this->statusBar()->showMessage("BTN_D3 released", 1000);
}

void Amp::on_btn4_pressed()
{
    this->bus |= BTN_D4;
    this->statusBar()->showMessage("BTN_D4 pressed", 1000);
}

void Amp::on_btn4_released()
{
    this->bus &= ~BTN_D4;
    this->statusBar()->showMessage("BTN_D4 released", 1000);
}

void Amp::on_btn5_pressed()
{
    this->bus |= BTN_D5;
    this->statusBar()->showMessage("BTN_D5 pressed", 1000);
}

void Amp::on_btn5_released()
{
    this->bus &= ~BTN_D5;
    this->statusBar()->showMessage("BTN_D5 released", 1000);
}

void Amp::on_dial_valueChanged(int value)
{
    dialChanged(value);
}

void Amp::on_dial_sliderMoved(int position)
{
    dialChanged(position);
}

void Amp::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_1:
        this->btn0->setDown(true);
        on_btn0_pressed();
        break;
    case Qt::Key_2:
        this->btn1->setDown(true);
        on_btn1_pressed();
        break;
    case Qt::Key_3:
        this->btn2->setDown(true);
        on_btn2_pressed();
        break;
    case Qt::Key_4:
        this->btn3->setDown(true);
        on_btn3_pressed();
        break;
    case Qt::Key_5:
        this->btn4->setDown(true);
        on_btn4_pressed();
        break;
    case Qt::Key_6:
        this->btn5->setDown(true);
        on_btn5_pressed();
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

void Amp::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_1:
        this->btn0->setDown(false);
        on_btn0_released();
        break;
    case Qt::Key_2:
        this->btn1->setDown(false);
        on_btn1_released();
        break;
    case Qt::Key_3:
        this->btn2->setDown(false);
        on_btn2_released();
        break;
    case Qt::Key_4:
        this->btn3->setDown(false);
        on_btn3_released();
        break;
    case Qt::Key_5:
        this->btn4->setDown(false);
        on_btn4_released();
        break;
    case Qt::Key_6:
        this->btn5->setDown(false);
        on_btn5_released();
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

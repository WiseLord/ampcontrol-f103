#include "amp.h"

#include "emuldisp.h"
#include "emulscreen.h"

#include <QDebug>

#include "../../../input.h"

static EmulDisp *disp;
static Amp *amp;

extern "C" void emulDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    disp->drawPixel(x, y, color);
}

extern "C" void emulDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    disp->drawRectangle(x, y, w, h, color);
}

extern "C" uint8_t dispdrvGetBus(void)
{
    return amp->bus;
}

extern "C" void dispdrvSetBrightness(int8_t value)
{
    disp->setBrightness(value);
}

Amp::Amp(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    disp = getEmulDisp();
    amp = this;

    this->bus = BTN_NO;

    disp->setFixedSize((EMUL_DISP_WIDTH + 2 * EMUL_DISP_BORDER) * EMUL_DISP_SCALE,
                       (EMUL_DISP_HEIGHT + 2 * EMUL_DISP_BORDER) * EMUL_DISP_SCALE);

    setFixedSize(sizeHint());
}

EmulDisp *Amp::getEmulDisp()
{
    return this->emulDisp;
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

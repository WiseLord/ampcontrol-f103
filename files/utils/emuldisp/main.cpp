#include <QApplication>

#include "amp.h"

#include "emuldisp.h"
#include "emulscreen.h"

#include "../../../eemul.h"
#include "../../../canvas/canvas.h"

static EmulDisp *disp;
static DispDriver drv;
static RcData rcData;

extern "C" void eeUpdate(EE_Param param, int16_t data)
{
    (void)param;
    (void)data;
}

extern "C" uint16_t rcGetCode(RcCmd cmd)
{
    (void)cmd;
    return 0;
}
extern "C" void rcSaveCode(uint16_t cmd, uint16_t value)
{
    (void)cmd, (void)value;
}
extern "C" RcData rcRead(bool clear)
{
    (void)clear;
    return rcData;
}

extern "C" void emulDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    disp->drawPixel(x, y, color);
}

extern "C" void emulDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    disp->drawRectangle(x, y, w, h, color);
}

extern "C" void dispdrvSetBrightness(int8_t value)
{
    (void) value;
}

extern "C" void swTimSetTunerPoll(int16_t value)
{
    (void) value;
}
extern "C" int16_t swTimGetTunerPoll(void)
{
    return 0;
}

extern "C" void swTimSetSpConvert(int16_t value)
{
    (void) value;
}
extern "C" int16_t swTimGetSpConvert(void)
{
    return 0;
}

extern "C" int8_t rtcGetMode(void)
{
    return RTC_HOUR;
}


void dispdrvInit(DispDriver **driver)
{
    drv.width = EMUL_DISP_WIDTH;
    drv.height = EMUL_DISP_HEIGHT;
    drv.drawPixel = emulDrawPixel;
    drv.drawRectangle = emulDrawRectangle;

    *driver = &drv;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Amp amp;

    disp = amp.getEmulDisp();
    disp->setFixedSize((EMUL_DISP_WIDTH + 2 * EMUL_DISP_BORDER) * EMUL_DISP_SCALE,
                       (EMUL_DISP_HEIGHT + 2 * EMUL_DISP_BORDER) * EMUL_DISP_SCALE);

    amp.setFixedSize(amp.sizeHint());
    amp.show();

    return a.exec();
}

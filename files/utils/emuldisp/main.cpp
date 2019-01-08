#include <QApplication>

#include "emuldisp.h"
#include "emulscreen.h"

#include "../../../eemul.h"
#include "../../../canvas/canvas.h"

static EmulDisp *disp;
static DispDriver drv;
static RcData rcData;

extern "C" void eeUpdate(EE_Param param, int16_t data) { (void)param, (void)data; }
extern "C" uint16_t eeReadU(EE_Param param, uint16_t def) { (void)param, (void)def; return 0; }
extern "C" int16_t eeReadI(EE_Param param, int16_t def) { (void)param, (void)def; return 0; }

extern "C" uint16_t rcGetCode(RcCmd cmd) { (void)cmd; return 0; }
extern "C" void rcSaveCode(uint16_t cmd, uint16_t value) { (void)cmd, (void)value; }
extern "C" RcData rcRead(bool clear) { (void)clear; return rcData; }

extern "C" void emulDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    disp->drawPixel(x, y, color);
}

void dispdrvInit(DispDriver **driver)
{
    drv.width = EMUL_DISP_WIDTH;
    drv.height = EMUL_DISP_HEIGHT;
    drv.drawPixel = emulDrawPixel;
    drv.drawImage = glcdDrawImage;

    *driver = &drv;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EmulDisp emulDisp;
    disp = &emulDisp;

    emulDisp.setSize(EMUL_DISP_WIDTH, EMUL_DISP_HEIGHT);
    emulDisp.show();

    return a.exec();
}

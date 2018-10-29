#include <QApplication>

#include "emuldisp.h"
#include "emulscreen.h"

#include "../../../eemul.h"
#include "../../../canvas/canvas.h"

static EmulDisp *disp;

static DispDriver drv;

extern "C" void eeUpdate(EE_Param param, int16_t data) {}
extern "C" uint16_t eeReadU(EE_Param param, uint16_t def) { return 0; }
extern "C" int16_t eeReadI(EE_Param param, int16_t def) { return 0; }

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

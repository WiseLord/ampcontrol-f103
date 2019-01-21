#include <QApplication>

#include "amp.h"

#include "emuldisp.h"
#include "emulscreen.h"

static EmulDisp *disp;

extern "C" void emulDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    disp->drawPixel(x, y, color);
}

extern "C" void emulDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    disp->drawRectangle(x, y, w, h, color);
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

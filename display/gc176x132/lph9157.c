#include "../dispdrv.h"

void lph9157Init(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x01);
    dispdrvSelectReg8(0x11);
    utilmDelay(20);
    dispdrvSelectReg8(0x3a);
    dispdrvSendData8(0x05);
    utilmDelay(20);
    dispdrvSelectReg8(0x36);
    dispdrvSendData8(0x40);
    dispdrvSelectReg8(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x10);
    dispdrvSelectReg8(0x28);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x11);
    dispdrvSelectReg8(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvSelectReg8(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2C);
}

const DispDriver dispdrv = {
    .width = 176,
    .height = 132,
    .init = lph9157Init,
    .sleep = lph9157Sleep,
    .wakeup = lph9157Wakeup,
    .setWindow = lph9157SetWindow,
};

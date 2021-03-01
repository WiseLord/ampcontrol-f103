#include "../dispdrv.h"

void st7796sInit(void)
{
    CLR(DISP_CS);

    //*************LCD Driver Initial **********//
    DISP_MDELAY(120); // Delay 120ms

    dispdrvSelectReg8(0x01);
    DISP_MDELAY(120); // Delay 120ms

    dispdrvSelectReg8(0x11); // Sleep Out

    DISP_MDELAY(120); // Delay 120ms


    dispdrvSelectReg8(0xf0); //Enable command 2
    dispdrvSendData8(0xc3);

    dispdrvSelectReg8(0xf0);
    dispdrvSendData8(0x96);

    dispdrvSelectReg8(0x36);
    dispdrvSendData8(0x40);

    dispdrvSelectReg8(0xB4); //1-dot inversion
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0xb6);
    dispdrvSendData8(0x20);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x3b);

    dispdrvSelectReg8(0xe8);
    dispdrvSendData8(0x40);
    dispdrvSendData8(0x8a);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x29);
    dispdrvSendData8(0x19);
    dispdrvSendData8(0xa5);
    dispdrvSendData8(0x33);

    dispdrvSelectReg8(0xc1);
    dispdrvSendData8(0x06);

    dispdrvSelectReg8(0xc2);
    dispdrvSendData8(0xa7);

    dispdrvSelectReg8(0xc5);
    dispdrvSendData8(0x18);

    dispdrvSelectReg8(0xe0); //Positive Voltage Gamma Control
    dispdrvSendData8(0xf0);
    dispdrvSendData8(0x09);
    dispdrvSendData8(0x0b);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x15);
    dispdrvSendData8(0x2f);
    dispdrvSendData8(0x54);
    dispdrvSendData8(0x42);
    dispdrvSendData8(0x3c);
    dispdrvSendData8(0x17);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0x18);
    dispdrvSendData8(0x1b);

    dispdrvSelectReg8(0xe1); //Negative Voltage Gamma Control
    dispdrvSendData8(0xf0);
    dispdrvSendData8(0x09);
    dispdrvSendData8(0x0b);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x2d);
    dispdrvSendData8(0x43);
    dispdrvSendData8(0x42);
    dispdrvSendData8(0x3b);
    dispdrvSendData8(0x16);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0x17);
    dispdrvSendData8(0x1b);

    dispdrvSelectReg8(0xf0); //Disable command 2
    dispdrvSendData8(0x3c);

    dispdrvSelectReg8(0xf0);
    dispdrvSendData8(0x69);

    DISP_MDELAY(120); //Delay 120ms

    dispdrvSelectReg8(0x29); //Display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void st7796sRotate(bool rotate)
{
    CLR(DISP_CS);

    if (rotate) {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x08);
    } else {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0xD8);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void st7796sShift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x33);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0xE0);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x37);
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void st7796sSleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
        dispdrvSelectReg8(0x28);    // Display OFF
        DISP_MDELAY(100);
        dispdrvSelectReg8(0x10);
    } else {
        dispdrvSelectReg8(0x11);    // Display ON
        DISP_MDELAY(100);
        dispdrvSelectReg8(0x29);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void st7796sSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .width = 480,
    .height = 320,
    .init = st7796sInit,
    .sleep = st7796sSleep,
    .setWindow = st7796sSetWindow,
    .rotate = st7796sRotate,
    .shift = st7796sShift,
};

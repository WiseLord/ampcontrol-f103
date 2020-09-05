#include "../dispdrv.h"

void hx8357cInit(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x01);    // Soft Reset
    DISP_MDELAY(150);
    dispdrvSelectReg8(0x28);    // Display OFF

    dispdrvSelectReg8(0x3A);    // Interface Pixel Format
    dispdrvSendData8(0x55);

//    dispdrvSelectReg8(0xB6);    // Display function control
//    dispdrvSendData8(0x00);     // BYPASS, RCM, RM, DM, PGT1, PGT0, PT1, PT0
//    dispdrvSendData8(0x62);     // 0, GS, SS, SM, ICS3, ICS2, ICS1, ICS0
//    dispdrvSendData8(0x3B);     // 0, 0, NL5, NL4, NL3, NL2, NL1, NL0

//    dispdrvSelectReg8(0x36);
//    dispdrvSendData8(0x08);     // MY, MX, MV, ML, RGB, MH, H_FLIP, V_FLIP

    dispdrvSelectReg8(0x7F);
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0x20);    // Display inversion off

    dispdrvSelectReg8(0x11);    // Sleep OUT
    DISP_MDELAY(120);
    dispdrvSelectReg8(0x29);    // Display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void hx8357cRotate(bool rotate)
{
    CLR(DISP_CS);

    if (rotate) {
        dispdrvSelectReg8(0xB6);
        dispdrvSendData8(0x00);
        dispdrvSendData8(0x02);
        dispdrvSendData8(0x3B);
    } else {
        dispdrvSelectReg8(0xB6);
        dispdrvSendData8(0x00);
        dispdrvSendData8(0x62);
        dispdrvSendData8(0x3B);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void hx8357cShift(int16_t value)
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

void hx8357cSleep(bool value)
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

    SET(DISP_CS);
}

void hx8357cSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .init = hx8357cInit,
    .sleep = hx8357cSleep,
    .setWindow = hx8357cSetWindow,
    .rotate = hx8357cRotate,
    .shift = hx8357cShift,
};

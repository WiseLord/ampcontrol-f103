#include "../dispdrv.h"

void s6d0144Init(void)
{
    CLR(DISP_CS);

    utilmDelay(1);
    dispdrvWriteReg16(0xB4, 0x0010);
    dispdrvWriteReg16(0x07, 0x0000);
    dispdrvWriteReg16(0x13, 0x0000);
    utilmDelay(1);
    dispdrvWriteReg16(0x12, 0x0071);
    dispdrvWriteReg16(0x13, 0x080B);
    dispdrvWriteReg16(0x14, 0x78DD);
    dispdrvWriteReg16(0x10, 0x1910);

    dispdrvWriteReg16(0x01, 0x0314);
    dispdrvWriteReg16(0x02, 0x0100);
    dispdrvWriteReg16(0x03, 0x0030);
    dispdrvWriteReg16(0x08, 0x0101);
    dispdrvWriteReg16(0x0b, 0x0005);
    dispdrvWriteReg16(0x0C, 0x0002);
    utilmDelay(10);


    dispdrvWriteReg16(0x30, 0x0000);
    dispdrvWriteReg16(0x31, 0x0506);
    dispdrvWriteReg16(0x32, 0x0403);
    dispdrvWriteReg16(0x33, 0x0200);
    dispdrvWriteReg16(0x34, 0x0303);
    dispdrvWriteReg16(0x35, 0x0002);
    dispdrvWriteReg16(0x36, 0x0707);
    dispdrvWriteReg16(0x37, 0x0200);
    dispdrvWriteReg16(0x38, 0x0900);
    dispdrvWriteReg16(0x11, 0x000A);

    dispdrvWriteReg16(0x40, 0x0000);
    dispdrvWriteReg16(0x41, 0x00E5);
    dispdrvWriteReg16(0x42, 0x9F00);
    dispdrvWriteReg16(0x43, 0x9F00);
    dispdrvWriteReg16(0x44, 0x7F00);
    dispdrvWriteReg16(0x45, 0x9f00);

    dispdrvWriteReg16(0x13, 0x081B);
    utilmDelay(50);
    dispdrvWriteReg16(0x07, 0x0037);      // DTE =1
    utilmDelay(20);

    SET(DISP_CS);
}

void s6d0144Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);    // Display OFF
    utilmDelay(50);
    dispdrvWriteReg16(0x0010, 0x0A01);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void s6d0144Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0010, 0x0A00);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    utilmDelay(50);
    dispdrvWriteReg16(0x0007, 0x1017);    // 65K color and display ON

    SET(DISP_CS);
}

void s6d0144SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0044, (uint16_t)(y | (y1 << 8)));
    dispdrvWriteReg16(0x0045, (uint16_t)(x | (x1 << 8)));

    // Set cursor
    dispdrvWriteReg16(0x0021, (uint16_t)(y | (x << 8)));

    // Select RAM mode
    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 160,
    .height = 128,
    .init = s6d0144Init,
    .sleep = s6d0144Sleep,
    .wakeup = s6d0144Wakeup,
    .setWindow = s6d0144SetWindow,
};

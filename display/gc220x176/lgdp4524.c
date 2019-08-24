#include "../dispdrv.h"

void lgdp4524Init(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);
    dispdrvWriteReg16(0x0012, 0x0000);
    dispdrvWriteReg16(0x0013, 0x0000);
    dispdrvWriteReg16(0x0014, 0x0331);

    utilmDelay(15); // Delay 15ms

    dispdrvWriteReg16(0x0012, 0x0009);
    dispdrvWriteReg16(0x0011, 0x0000);
    dispdrvWriteReg16(0x0010, 0x0104);
    dispdrvWriteReg16(0x0013, 0x0C31);
    dispdrvWriteReg16(0x0010, 0x0144);
    dispdrvWriteReg16(0x0011, 0x0200);
    dispdrvWriteReg16(0x0012, 0x0019);

    utilmDelay(20); // Delay 20ms

    dispdrvWriteReg16(0x0013, 0x2C31);
    dispdrvWriteReg16(0x0010, 0x0140);

    utilmDelay(20); // Delay 20ms

    dispdrvWriteReg16(0x0060, 0x2000);
    dispdrvWriteReg16(0x0060, 0x0000);
    dispdrvWriteReg16(0x0061, 0x0002);
    dispdrvWriteReg16(0x0001, 0x031B);
    dispdrvWriteReg16(0x0002, 0x0700);
    dispdrvWriteReg16(0x0003, 0x1030);
    dispdrvWriteReg16(0x0008, 0x0304);
    dispdrvWriteReg16(0x0009, 0x0000);
    dispdrvWriteReg16(0x000B, 0x5D2C);
    dispdrvWriteReg16(0x0021, 0x0000);
    dispdrvWriteReg16(0x0030, 0x0101);
    dispdrvWriteReg16(0x0031, 0x0604);
    dispdrvWriteReg16(0x0032, 0x0007);
    dispdrvWriteReg16(0x0033, 0x0403);
    dispdrvWriteReg16(0x0034, 0x0007);
    dispdrvWriteReg16(0x0035, 0x0402);
    dispdrvWriteReg16(0x0036, 0x0606);
    dispdrvWriteReg16(0x0037, 0x0304);
    dispdrvWriteReg16(0x0038, 0x070E);
    dispdrvWriteReg16(0x0039, 0x070E);
    dispdrvWriteReg16(0x003A, 0x0101);
    dispdrvWriteReg16(0x003B, 0x0101);
    dispdrvWriteReg16(0x003C, 0x0101);
    dispdrvWriteReg16(0x003D, 0x0101);
    dispdrvWriteReg16(0x003E, 0x0101);
    dispdrvWriteReg16(0x003F, 0x0101);

    dispdrvWriteReg16(0x0042, 0xDB00);
    dispdrvWriteReg16(0x0044, 0xAF00);
    dispdrvWriteReg16(0x0045, 0xDB00);
    dispdrvWriteReg16(0x0071, 0x0040);
    dispdrvWriteReg16(0x0072, 0x0002);

    utilmDelay(10); // Delay 10ms

    dispdrvWriteReg16(0x0010, 0x3250);
    dispdrvWriteReg16(0x0007, 0x0001);

    utilmDelay(30); // Delay 30ms

    dispdrvWriteReg16(0x0007, 0x0021);
    dispdrvWriteReg16(0x0007, 0x0023);

    utilmDelay(30); // Delay 30ms

    dispdrvWriteReg16(0x0007, 0x0037);  // 65K color and display ON*/

    SET(DISP_CS);
}

void lgdp4524Sleep(void)
{
    CLR(DISP_CS);


    dispdrvWriteReg16(0x0007, 0x0036);

    utilmDelay(20);

    dispdrvWriteReg16(0x0007, 0x0026);

    utilmDelay(20);

    dispdrvWriteReg16(0x0007, 0x0000);

    utilmDelay(10);

    dispdrvWriteReg16(0x0010, 0x0004);
    dispdrvWriteReg16(0x0012, 0x0000);
    dispdrvWriteReg16(0x0013, 0x0000);

    utilmDelay(10);

    dispdrvWriteReg16(0x0010, 0x0008);

    SET(DISP_CS);
}

void lgdp4524Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0000, 0x0001);

    utilmDelay(10); // Delay 10ms

    dispdrvWriteReg16(0x0010, 0x0000);
    dispdrvWriteReg16(0x0014, 0x0331);

    utilmDelay(15); // Delay 15ms

    dispdrvWriteReg16(0x0012, 0x0009);
    dispdrvWriteReg16(0x0011, 0x0000);
    dispdrvWriteReg16(0x0010, 0x0104);
    dispdrvWriteReg16(0x0013, 0x0C31);
    dispdrvWriteReg16(0x0010, 0x0144);
    dispdrvWriteReg16(0x0011, 0x0200);
    dispdrvWriteReg16(0x0012, 0x0019);

    utilmDelay(20); // Delay 20ms

    dispdrvWriteReg16(0x0013, 0x2C31);
    dispdrvWriteReg16(0x0010, 0x0140);

    utilmDelay(20); // Delay 20ms

    dispdrvWriteReg16(0x0010, 0x3250);
    dispdrvWriteReg16(0x0007, 0x0001);

    utilmDelay(30); // Delay 30ms

    dispdrvWriteReg16(0x0007, 0x0021);
    dispdrvWriteReg16(0x0007, 0x0023);

    utilmDelay(30); // Delay 30ms

    dispdrvWriteReg16(0x0007, 0x0037);  // 65K color and display ON*/

    SET(DISP_CS);
}

void lgdp4524SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .width = 220,
    .height = 176,
    .init = lgdp4524Init,
    .sleep = lgdp4524Sleep,
    .wakeup = lgdp4524Wakeup,
    .setWindow = lgdp4524SetWindow,
};

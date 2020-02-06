#include "../dispdrv.h"

void s6d0129Init(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x00e5, 0x8000);

    dispdrvWriteReg16(0x0000, 0x0001);
    utilmDelay(100);

    dispdrvWriteReg16(0x0011, 0x2E00);
    dispdrvWriteReg16(0x0014, 0x040B);
    dispdrvWriteReg16(0x0010, 0x1040);
    utilmDelay(10);
    dispdrvWriteReg16(0x0013, 0x0040);
    utilmDelay(10);
    dispdrvWriteReg16(0x0013, 0x0060);
    utilmDelay(10);
    dispdrvWriteReg16(0x0013, 0x0070);
    utilmDelay(6);
    dispdrvWriteReg16(0x0011, 0x3704);
    dispdrvWriteReg16(0x0010, 0x1600);
    utilmDelay(2);
    dispdrvWriteReg16(0x0001, 0x0B27);
    dispdrvWriteReg16(0x0002, 0x0700);
    dispdrvWriteReg16(0x0003, 0x1030);
    dispdrvWriteReg16(0x0007, 0x0004);
    dispdrvWriteReg16(0x0008, 0x0505);
    dispdrvWriteReg16(0x0009, 0x0000);
    dispdrvWriteReg16(0x000B, 0x0000);
    dispdrvWriteReg16(0x000C, 0x0000);


    dispdrvWriteReg16(0x0040, 0x0000);
//    s6d0129WriteReg(0x0042,0x013F);
//    s6d0129WriteReg(0x0043,0x0000);
//    s6d0129WriteReg(0x0044,0x013F);
//    s6d0129WriteReg(0x0045,0x0000);
    dispdrvWriteReg16(0x0046, 0xEF00);
    dispdrvWriteReg16(0x0047, 0x013F);
    dispdrvWriteReg16(0x0048, 0x0000);
    utilmDelay(5);

    dispdrvWriteReg16(0x0030, 0x0000);
    dispdrvWriteReg16(0x0031, 0x0006);
    dispdrvWriteReg16(0x0032, 0x0000);
    dispdrvWriteReg16(0x0033, 0x0000);
    dispdrvWriteReg16(0x0034, 0x0707);
    dispdrvWriteReg16(0x0035, 0x0700);
    dispdrvWriteReg16(0x0036, 0x0303);
    dispdrvWriteReg16(0x0037, 0x0007);
    dispdrvWriteReg16(0x0038, 0x1100);
    dispdrvWriteReg16(0x0039, 0x1100);

    dispdrvWriteReg16(0x0007, 0x0015);
    utilmDelay(5);
    dispdrvWriteReg16(0x0007, 0x0017);
    utilmDelay(5);

    SET(DISP_CS);
}

void s6d0129Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvWriteReg16(0x0001, 0x0827);
    } else {
        dispdrvWriteReg16(0x0001, 0x0B27);
    }

    SET(DISP_CS);
}

void s6d0129Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    dispdrvWriteReg16(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    dispdrvWriteReg16(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    dispdrvWriteReg16(0x0013, 0x0000);    // VREG1OUT voltage

    dispdrvWriteReg16(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    utilmDelay(200);
    dispdrvWriteReg16(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void s6d0129Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    dispdrvWriteReg16(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    dispdrvWriteReg16(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    dispdrvWriteReg16(0x0013, 0x0000);    // VREG1OUT voltage
    dispdrvWriteReg16(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    utilmDelay(200);
    dispdrvWriteReg16(0x0007, 0x0000);    // Display control1
    dispdrvWriteReg16(0x0013, 0x0000);    // Power control4 setting
    dispdrvWriteReg16(0x0011, 0x2604);    // Power control2 setting
    dispdrvWriteReg16(0x0014, 0x0015);    // Power control5 setting
    dispdrvWriteReg16(0x0010, 0x3C00);    // Power control1 setting
    dispdrvWriteReg16(0x0013, 0x0040);    // Power control4 setting
    utilmDelay(10);
    dispdrvWriteReg16(0x0013, 0x0060);    // Power control4 setting
    utilmDelay(50);
    dispdrvWriteReg16(0x0013, 0x0070);    // Power control4 setting
    utilmDelay(40);
    dispdrvWriteReg16(0x0007, 0x0017);    // 262K color and display ON

    SET(DISP_CS);
}

void s6d0129SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0046, (uint16_t)(y | (y1 << 8)));
    dispdrvWriteReg16(0x0048, (uint16_t)x);
    dispdrvWriteReg16(0x0047, (uint16_t)x1);

    dispdrvWriteReg16(0x0020, (uint16_t)y);
    dispdrvWriteReg16(0x0021, (uint16_t)x);

    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = s6d0129Init,
    .sleep = s6d0129Sleep,
    .wakeup = s6d0129Wakeup,
    .setWindow = s6d0129SetWindow,
    .rotate = s6d0129Rotate,
};

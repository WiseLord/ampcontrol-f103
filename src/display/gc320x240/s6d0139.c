#include "../dispdrv.h"

void s6d0139Init(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0000, 0x0001);    // Start Oscillation
    utilmDelay(10);
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

    dispdrvWriteReg16(0x0001, 0x0327);    // Driver output setting (240x320 mode, GS=1, SS=1)
    dispdrvWriteReg16(0x0002, 0x0700);    // LCD driving waveform setting
    dispdrvWriteReg16(0x0003, 0x1030);    // Entry mode setting (TRI=0, DFM=0, BGR=1, ID1:ID0 =11)
    dispdrvWriteReg16(0x0007, 0x0000);    // Display control1
    dispdrvWriteReg16(0x0008, 0x0202);    // Display control2
    dispdrvWriteReg16(0x000B, 0x0200);    // Frame cycle setting
    dispdrvWriteReg16(0x000C, 0x0000);    // External display interface setting
    dispdrvWriteReg16(0x0015, 0x0000);

    //gamma setting
    dispdrvWriteReg16(0x0030, 0x0000);    // Gamma control1
    dispdrvWriteReg16(0x0031, 0x0606);    // Gamma control2
    dispdrvWriteReg16(0x0032, 0x0006);    // Gamma control3
    dispdrvWriteReg16(0x0033, 0x0403);    // Gamma control4
    dispdrvWriteReg16(0x0034, 0x0107);    // Gamma control5
    dispdrvWriteReg16(0x0035, 0x0101);    // Gamma control6
    dispdrvWriteReg16(0x0036, 0x0707);    // Gamma control7
    dispdrvWriteReg16(0x0037, 0x0304);    // Gamma control8
    dispdrvWriteReg16(0x0038, 0x0A00);    // Gamma control9
    dispdrvWriteReg16(0x0039, 0x0706);    // Gamma control10

    dispdrvWriteReg16(0x0040, 0x0000);    // Gate scan position (start G1 scan)
    dispdrvWriteReg16(0x0041, 0x0000);    // Vertical scroll setting
    dispdrvWriteReg16(0x0042, 0x013F);    // Screen end position (320-1)
    dispdrvWriteReg16(0x0043, 0x0000);    // Screen start position (0)
    dispdrvWriteReg16(0x0044, 0x0000);    // 2th Screen end position
    dispdrvWriteReg16(0x0045, 0x0000);    // 2th Screen start position
    dispdrvWriteReg16(0x0046, 0xEF00);    // window addr set for x0,x1 (0, 239)
    dispdrvWriteReg16(0x0047, 0x013F);    // window addr set for y0    (319)
    dispdrvWriteReg16(0x0048, 0x0000);    // window addr set for y1    (0)
    dispdrvWriteReg16(0x0007, 0x0011);    // Display control1
    utilmDelay(40);
    dispdrvWriteReg16(0x0007, 0x0017);    // Display control1

    SET(DISP_CS);
}

void s6d0139Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvWriteReg16(0x0001, 0x0027);
    } else {
        dispdrvWriteReg16(0x0001, 0x0327);
    }

    SET(DISP_CS);
}

void s6d0139Sleep(void)
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

void s6d0139Wakeup(void)
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

void s6d0139SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .init = s6d0139Init,
    .sleep = s6d0139Sleep,
    .wakeup = s6d0139Wakeup,
    .setWindow = s6d0139SetWindow,
    .rotate = s6d0139Rotate,
};

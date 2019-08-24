#include "../dispdrv.h"

void ssd1289Init(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0000, 0x0001);
    dispdrvWriteReg16(0x0003, 0xA8A4);
    dispdrvWriteReg16(0x000C, 0x0000);
    dispdrvWriteReg16(0x000D, 0x080C);
    dispdrvWriteReg16(0x000E, 0x2B00);
    dispdrvWriteReg16(0x001E, 0x00B7);
    dispdrvWriteReg16(0x0001, 0x293F);
    dispdrvWriteReg16(0x0002, 0x0600);
    dispdrvWriteReg16(0x0010, 0x0000);
    dispdrvWriteReg16(0x0011, 0x6070);
    dispdrvWriteReg16(0x0005, 0x0000);
    dispdrvWriteReg16(0x0006, 0x0000);
    dispdrvWriteReg16(0x0016, 0xEF1C);
    dispdrvWriteReg16(0x0017, 0x0003);
    dispdrvWriteReg16(0x0007, 0x0233);
    dispdrvWriteReg16(0x000B, 0x0000);
    dispdrvWriteReg16(0x000F, 0x0000);

    dispdrvWriteReg16(0x0041, 0x0000);
    dispdrvWriteReg16(0x0042, 0x0000);
    dispdrvWriteReg16(0x0048, 0x0000);
    dispdrvWriteReg16(0x0049, 0x013F);
    dispdrvWriteReg16(0x004A, 0x0000);
    dispdrvWriteReg16(0x004B, 0x0000);
    dispdrvWriteReg16(0x0044, 0xEF00);
    dispdrvWriteReg16(0x0045, 0x0000);
    dispdrvWriteReg16(0x0046, 0x013F);

    // Adjust the Gamma Curve
    dispdrvWriteReg16(0x0030, 0x0707);
    dispdrvWriteReg16(0x0031, 0x0204);
    dispdrvWriteReg16(0x0032, 0x0204);
    dispdrvWriteReg16(0x0033, 0x0502);
    dispdrvWriteReg16(0x0034, 0x0507);
    dispdrvWriteReg16(0x0035, 0x0204);
    dispdrvWriteReg16(0x0036, 0x0204);
    dispdrvWriteReg16(0x0037, 0x0502);
    dispdrvWriteReg16(0x003A, 0x0302);
    dispdrvWriteReg16(0x003B, 0x0302);

    dispdrvWriteReg16(0x0023, 0x0000);
    dispdrvWriteReg16(0x0024, 0x0000);
    dispdrvWriteReg16(0x0025, 0x8000);
    dispdrvWriteReg16(0x004f, 0x0000);
    dispdrvWriteReg16(0x004e, 0x0000);

    SET(DISP_CS);
}

void ssd1289Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvWriteReg16(0x0001, 0x6B3F);
    } else {
        dispdrvWriteReg16(0x0001, 0x293F);

    }

    SET(DISP_CS);
}

void ssd1289Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x006A, (uint16_t)value);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1289Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    dispdrvWriteReg16(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    dispdrvWriteReg16(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    dispdrvWriteReg16(0x0012, 0x0000);    // VREG1OUT voltage
    dispdrvWriteReg16(0x0013, 0x0000);    // VDV[4:0] for VCOM amplitude
    utilmDelay(200);
    dispdrvWriteReg16(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1289Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    dispdrvWriteReg16(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    dispdrvWriteReg16(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    dispdrvWriteReg16(0x0012, 0x0000);    // VREG1OUT voltage
    dispdrvWriteReg16(0x0013, 0x0000);    // VDV[4:0] for VCOM amplitude
    utilmDelay(200);
    dispdrvWriteReg16(0x0010, 0x17B0);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    dispdrvWriteReg16(0x0011, 0x0037);    // DC1[2:0], DC0[2:0], VC[2:0]
    utilmDelay(50);
    dispdrvWriteReg16(0x0012, 0x013A);    // VREG1OUT voltage
    utilmDelay(50);
    dispdrvWriteReg16(0x0013, 0x1600);    // VDV[4:0] for VCOM amplitude
    dispdrvWriteReg16(0x0029, 0x000C);    // VCM[4:0] for VCOMH

    dispdrvWriteReg16(0x0007, 0x0173);    // 262K color and display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1289SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0044, (uint16_t)(y | (y1 << 8)));
    dispdrvWriteReg16(0x0045, (uint16_t)x);
    dispdrvWriteReg16(0x0046, (uint16_t)x1);

    // Set cursor
    dispdrvWriteReg16(0x004E, (uint16_t)y);
    dispdrvWriteReg16(0x004F, (uint16_t)x);

    // Select RAM mode
    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = ssd1289Init,
    .sleep = ssd1289Sleep,
    .wakeup = ssd1289Wakeup,
    .setWindow = ssd1289SetWindow,
    .rotate = ssd1289Rotate,
    .shift = ssd1289Shift,
};

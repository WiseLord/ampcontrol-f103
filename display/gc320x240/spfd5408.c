#include "../dispdrv.h"

void spfd5408Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    dispdrvWriteReg16(0x0000, 0x0000);
    dispdrvWriteReg16(0x0001, 0x0100);
    dispdrvWriteReg16(0x0002, 0x0700);
    dispdrvWriteReg16(0x0003, 0x1030);
    dispdrvWriteReg16(0x0004, 0x0000);
    dispdrvWriteReg16(0x0008, 0x0207);
    dispdrvWriteReg16(0x0009, 0x0000);
    dispdrvWriteReg16(0x000A, 0x0000);
    dispdrvWriteReg16(0x000C, 0x0000);
    dispdrvWriteReg16(0x000D, 0x0000);
    dispdrvWriteReg16(0x000F, 0x0000);
    dispdrvWriteReg16(0x0007, 0x0101);

    // Power On Sequence
    dispdrvWriteReg16(0x0010, 0x14B0);
    utilmDelay(20);
    dispdrvWriteReg16(0x0011, 0x0007);
    utilmDelay(20);
    dispdrvWriteReg16(0x0017, 0x0001);
    utilmDelay(20);
    dispdrvWriteReg16(0x0012, 0x01B8);
    utilmDelay(20);
    dispdrvWriteReg16(0x0013, 0x1300);
    utilmDelay(20);
    dispdrvWriteReg16(0x0029, 0x000F);

    // Adjust the Gamma Curve
    dispdrvWriteReg16(0x0030, 0x0102);
    dispdrvWriteReg16(0x0031, 0x0C18);
    dispdrvWriteReg16(0x0032, 0x0F1C);
    dispdrvWriteReg16(0x0033, 0x3617);
    dispdrvWriteReg16(0x0034, 0x350B);
    dispdrvWriteReg16(0x0035, 0x1004);
    dispdrvWriteReg16(0x0036, 0x0D06);
    dispdrvWriteReg16(0x0037, 0x0318);
    dispdrvWriteReg16(0x0038, 0x0604);
    dispdrvWriteReg16(0x0039, 0x0005);
    dispdrvWriteReg16(0x003A, 0x0A05);
    dispdrvWriteReg16(0x003B, 0x0A00);
    dispdrvWriteReg16(0x003C, 0x020F);
    dispdrvWriteReg16(0x003D, 0x050F);
    dispdrvWriteReg16(0x003E, 0x0303);
    dispdrvWriteReg16(0x003F, 0x0505);

    // Set GRAM area
    dispdrvWriteReg16(0x0020, 0x0000);   // GRAM horisontal address
    dispdrvWriteReg16(0x0021, 0x0000);   // GRAM vertical address

    dispdrvWriteReg16(0x0050, 0x0000);   // Horizontal GRAM Start Address
    dispdrvWriteReg16(0x0051, 0x00EF);   // Horizontal GRAM End Address
    dispdrvWriteReg16(0x0052, 0x0000);   // Vertical GRAM Start Address
    dispdrvWriteReg16(0x0053, 0x013F);   // Vertical GRAM End Address

    dispdrvWriteReg16(0x0060, 0xA700);   // Gate scan line
    dispdrvWriteReg16(0x0061, 0x0003);   // NDV, VLE, REV
    dispdrvWriteReg16(0x006A, 0x0000);   // Set scrolling line

    // Partial Display Control
    dispdrvWriteReg16(0x0080, 0x0000);
    dispdrvWriteReg16(0x0081, 0x0000);
    dispdrvWriteReg16(0x0082, 0x0000);
    dispdrvWriteReg16(0x0083, 0x0000);
    dispdrvWriteReg16(0x0084, 0x0000);
    dispdrvWriteReg16(0x0085, 0x0000);

    // Panel Control
    dispdrvWriteReg16(0x0090, 0x0018);
    dispdrvWriteReg16(0x0092, 0x0000);
    dispdrvWriteReg16(0x0093, 0x0103);
    dispdrvWriteReg16(0x0095, 0x0110);
    dispdrvWriteReg16(0x0097, 0x0000);
    dispdrvWriteReg16(0x0098, 0x0000);

    dispdrvWriteReg16(0x00F0, 0x5408);
    dispdrvWriteReg16(0x00F2, 0x00DF);
    dispdrvWriteReg16(0x00F3, 0x0006);
    dispdrvWriteReg16(0x00F4, 0x001F);
    dispdrvWriteReg16(0x00F7, 0x0008);
    dispdrvWriteReg16(0x00F0, 0x0000);

    dispdrvWriteReg16(0x0007, 0x0173);   // 262K color and display ON
    utilmDelay(150);

    SET(DISP_CS);
}

void spfd5408Sleep(void)
{
    dispdrvWriteReg16(0x0007, 0x0000);   // Display OFF
    // Power Off Sequence
    dispdrvWriteReg16(0x0010, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0011, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0017, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0012, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0013, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0010, 0x0002);   // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void spfd5408Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    dispdrvWriteReg16(0x0010, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0011, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0017, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0012, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0013, 0x0000);
    utilmDelay(20);
    dispdrvWriteReg16(0x0010, 0x14B0);
    utilmDelay(20);
    dispdrvWriteReg16(0x0011, 0x0007);
    utilmDelay(20);
    dispdrvWriteReg16(0x0017, 0x0001);
    utilmDelay(20);
    dispdrvWriteReg16(0x0012, 0x01B8);
    utilmDelay(20);
    dispdrvWriteReg16(0x0013, 0x1300);
    utilmDelay(20);
    dispdrvWriteReg16(0x0029, 0x000F);

    dispdrvWriteReg16(0x0007, 0x0173);   // 262K color and display ON

    SET(DISP_CS);
}

void spfd5408SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0050, (uint16_t)y);
    dispdrvWriteReg16(0x0051, (uint16_t)y1);
    dispdrvWriteReg16(0x0052, (uint16_t)x);
    dispdrvWriteReg16(0x0053, (uint16_t)x1);

    // Set cursor
    dispdrvWriteReg16(0x0020, (uint16_t)y);
    dispdrvWriteReg16(0x0021, (uint16_t)x);

    // Set RAM mode
    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = spfd5408Init,
    .sleep = spfd5408Sleep,
    .wakeup = spfd5408Wakeup,
    .setWindow = spfd5408SetWindow,
};

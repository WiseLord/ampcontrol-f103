#include "../dispdrv.h"

void ili9320Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    dispdrvWriteReg16(0x00E5, 0x8000);    // Set the Vcore voltage and this setting is must
    dispdrvWriteReg16(0x0000, 0x0001);    // Start internal OSC
    dispdrvWriteReg16(0x0001, 0x0100);    // Set SS and SM bit
    dispdrvWriteReg16(0x0002, 0x0700);    // Set 1 line inversion
    dispdrvWriteReg16(0x0003, 0x1030);    // Set GRAM write direction and BGR = 1
    dispdrvWriteReg16(0x0004, 0x0000);    // Resize register

    dispdrvWriteReg16(0x0008, 0x0202);    // Set the back porch and front porch
    dispdrvWriteReg16(0x0009, 0x0000);    // Set non-display area refresh cycle ICS[3:0]
    dispdrvWriteReg16(0x000A, 0x0000);    // FMARK function
    dispdrvWriteReg16(0x000C, 0x0000);    // RGB interface setting
    dispdrvWriteReg16(0x000D, 0x0000);    // Frame marker position
    dispdrvWriteReg16(0x000F, 0x0000);    // RGB interface polarity

    // Power On Sequence
    dispdrvWriteReg16(0x0010, 0x17B0);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    dispdrvWriteReg16(0x0011, 0x0037);    // DC1[2:0], DC0[2:0], VC[2:0]
    dispdrvWriteReg16(0x0012, 0x013A);    // VREG1OUT voltage
    dispdrvWriteReg16(0x0013, 0x1600);    // VDV[4:0] for VCOM amplitude
    dispdrvWriteReg16(0x0029, 0x000C);    // VCM[4:0] for VCOMH

    // Adjust the Gamma Curve
    dispdrvWriteReg16(0x0030, 0x0007);
    dispdrvWriteReg16(0x0031, 0x0700);
    dispdrvWriteReg16(0x0032, 0x0707);
    dispdrvWriteReg16(0x0035, 0x0007);
    dispdrvWriteReg16(0x0036, 0x000F);
    dispdrvWriteReg16(0x0037, 0x0700);
    dispdrvWriteReg16(0x0038, 0x0707);
    dispdrvWriteReg16(0x0039, 0x0007);
    dispdrvWriteReg16(0x003C, 0x0700);
    dispdrvWriteReg16(0x003D, 0x0F00);

    // Set GRAM area
    dispdrvWriteReg16(0x0020, 0x0000);    // GRAM horisontal address
    dispdrvWriteReg16(0x0021, 0x0000);    // GRAM vertical address

    dispdrvWriteReg16(0x0050, 0x0000);    // Horizontal GRAM Start Address
    dispdrvWriteReg16(0x0051, 0x00EF);    // Horizontal GRAM End Address
    dispdrvWriteReg16(0x0052, 0x0000);    // Vertical GRAM Start Address
    dispdrvWriteReg16(0x0053, 0x013F);    // Vertical GRAM End Address

    dispdrvWriteReg16(0x0060, 0x2700);    // Gate scan line
    dispdrvWriteReg16(0x0061, 0x0003);    // NDV, VLE, REV
    dispdrvWriteReg16(0x006A, 0x0000);    // Set scrolling line

    // Partial Display Control
    dispdrvWriteReg16(0x0080, 0x0000);
    dispdrvWriteReg16(0x0081, 0x0000);
    dispdrvWriteReg16(0x0082, 0x0000);
    dispdrvWriteReg16(0x0083, 0x0000);
    dispdrvWriteReg16(0x0084, 0x0000);
    dispdrvWriteReg16(0x0085, 0x0000);

    // Panel Control
    dispdrvWriteReg16(0x0090, 0x0010);
    dispdrvWriteReg16(0x0092, 0x0000);
    dispdrvWriteReg16(0x0093, 0x0003);
    dispdrvWriteReg16(0x0095, 0x0110);
    dispdrvWriteReg16(0x0097, 0x0000);
    dispdrvWriteReg16(0x0098, 0x0000);

    dispdrvWriteReg16(0x0007, 0x0173);    // 262K color and display ON

    SET(DISP_CS);
}

void ili9320Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvWriteReg16(0x0001, 0x0000);    // Set SS and SM bit
        dispdrvWriteReg16(0x0060, 0xA700);    // Gate scan line
    } else {
        dispdrvWriteReg16(0x0001, 0x0100);    // Set SS and SM bit
        dispdrvWriteReg16(0x0060, 0x2700);    // Gate scan line
    }

    SET(DISP_CS);
}

void ili9320Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x006A, (uint16_t)value);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9320Sleep(void)
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

void ili9320Wakeup(void)
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

void ili9320SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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

    // Select RAM mode
    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = ili9320Init,
    .sleep = ili9320Sleep,
    .wakeup = ili9320Wakeup,
    .setWindow = ili9320SetWindow,
    .rotate = ili9320Rotate,
    .shift = ili9320Shift,
};

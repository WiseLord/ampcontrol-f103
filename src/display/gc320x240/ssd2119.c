#include "../dispdrv.h"

void ssd2119Init(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0028, 0x0006);    // VCOM OTP
    dispdrvWriteReg16(0x0000, 0x0001);    // Start internal OSC
    dispdrvWriteReg16(0x0001, 0x72EF);    // Driver Output Control
    dispdrvWriteReg16(0x0002, 0x0400);
    dispdrvWriteReg16(0x0003, 0x6064);    // Sleep=0
    dispdrvWriteReg16(0x0010, 0x0000);

    dispdrvWriteReg16(0x0011, 0x6238);
    dispdrvWriteReg16(0x0007, 0x0000);
    dispdrvWriteReg16(0x0025, 0xD000);
    dispdrvWriteReg16(0x000B, 0x53c8);

    // Adjust the Gamma Curve
    dispdrvWriteReg16(0x0030, 0x0000);
    dispdrvWriteReg16(0x0031, 0x0101);
    dispdrvWriteReg16(0x0032, 0x0100);
    dispdrvWriteReg16(0x0033, 0x0305);
    dispdrvWriteReg16(0x0034, 0x0707);
    dispdrvWriteReg16(0x0035, 0x0305);
    dispdrvWriteReg16(0x0036, 0x0707);
    dispdrvWriteReg16(0x0037, 0x0201);
    dispdrvWriteReg16(0x003A, 0x1200);
    dispdrvWriteReg16(0x003B, 0x0900);

    // Power On Sequence
    dispdrvWriteReg16(0x000C, 0x0004);
    utilmDelay(50);
    dispdrvWriteReg16(0x000D, 0x0009);
    utilmDelay(50);
    dispdrvWriteReg16(0x001E, 0x0068);
    utilmDelay(50);
    dispdrvWriteReg16(0x000E, 0x2700);
    utilmDelay(50);
    dispdrvWriteReg16(0x0026, 0x7C00);
    dispdrvWriteReg16(0x0027, 0x006D);

    // Set GRAM area
    dispdrvWriteReg16(0x004E, 0x0000);    // GRAM horisontal address
    dispdrvWriteReg16(0x004F, 0x0000);    // GRAM vertical address

    dispdrvWriteReg16(0x0044, 0xEF00);    // Vertical GRAM End Start Address
    dispdrvWriteReg16(0x0045, 0x0000);    // Horizontal GRAM Start Address
    dispdrvWriteReg16(0x0046, 0x013F);    // Horizontal GRAM End Address

    dispdrvWriteReg16(0x0007, 0x0033);    // 65K color and display ON*/

    SET(DISP_CS);
}

void ssd2119Rotate(uint8_t rotate)
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

void ssd2119Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x006A, (uint16_t)value);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);    // Display OFF
    dispdrvWriteReg16(0x0010, 0x0002);    // Sleep=0

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    dispdrvWriteReg16(0x0010, 0x0000);    // Sleep=0
    dispdrvWriteReg16(0x0007, 0x0173);    // display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0044, (uint16_t)(y | (y1 << 8)));
    dispdrvWriteReg16(0x0045, (uint16_t)x);
    dispdrvWriteReg16(0x0046, (uint16_t)x1);

    // Set cursor
    dispdrvWriteReg16(0x004E, (uint16_t)x);
    dispdrvWriteReg16(0x004F, (uint16_t)y);

    // Select RAM mode
    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = ssd2119Init,
    .sleep = ssd2119Sleep,
    .wakeup = ssd2119Wakeup,
    .setWindow = ssd2119SetWindow,
    .rotate = ssd2119Rotate,
    .shift = ssd2119Shift,
};

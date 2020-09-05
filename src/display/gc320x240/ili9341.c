#include "../dispdrv.h"

void ili9341Init(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x01); // Software Reset
    DISP_MDELAY(10);
    dispdrvSelectReg8(0x28); // Display OFF

    dispdrvSelectReg8(0xCF); // Power Control B
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x30);

    dispdrvSelectReg8(0xED); // Power On Sequence Control
    dispdrvSendData8(0x64);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x81);

    dispdrvSelectReg8(0xE8); // Driver Timing Control A 1
    dispdrvSendData8(0x85);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x79);

    dispdrvSelectReg8(0xCB); // Power Control A
    dispdrvSendData8(0x39);
    dispdrvSendData8(0x2C);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x02);

    dispdrvSelectReg8(0xF7); // Pump Ratio Control
    dispdrvSendData8(0x20);

    dispdrvSelectReg8(0xEA); // Driver Timing Control B
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xC0); // Power Control 1
    dispdrvSendData8(0x26);

    dispdrvSelectReg8(0xC1); // Power Control 2
    dispdrvSendData8(0x11);

    dispdrvSelectReg8(0xC5); // VCOM Control 1
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x3E);

    dispdrvSelectReg8(0xC7); // VCOM Control 2
    dispdrvSendData8(0xBE);

    dispdrvSelectReg8(0x3A); // COLMOD: Pixel Format Set
    dispdrvSendData8(0x55);

    dispdrvSelectReg8(0xB1); // Frame Rate Control (In Normal Mode / Full Colors)
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x1B);

    dispdrvSelectReg8(0xF2); // Enable 3G
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0x26); // Gamma Set
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0xB7); // Entry Mode Set
    dispdrvSendData8(0x07);

    dispdrvSelectReg8(0xB6); // Display Function Control
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0xE2);
    dispdrvSendData8(0x27);

    dispdrvSelectReg8(0xB8); // Hack for unknown display with ID 1602
    dispdrvSendData8(0x01);

    // Rotate
    dispdrvSelectReg8(0x36); // Memory Access Control
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0xE0); // Positive Gamma Control
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x2B);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x4E);
    dispdrvSendData8(0xF1);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x09);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xE1); // Negative Gamma Control
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x48);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x36);
    dispdrvSendData8(0x0F);

    dispdrvSelectReg8(0x11); // Sleep Out
    dispdrvSelectReg8(0x29); // Display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Rotate(bool rotate)
{
    CLR(DISP_CS);

    if (rotate) {
        dispdrvSelectReg8(0xB6);
        dispdrvSendData8(0x0A);
        dispdrvSendData8(0x82);
    } else {
        dispdrvSelectReg8(0xB6);
        dispdrvSendData8(0x0A);
        dispdrvSendData8(0xE2);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x33); // Vertical Scrolling Definition
    dispdrvSendData8(0);
    dispdrvSendData8(0);
    dispdrvSendData8((dispdrv.width >> 8) & 0xFF);
    dispdrvSendData8(dispdrv.width & 0xFF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);

    dispdrvSelectReg8(0x37); // Vertical Scrolling Start Address
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Sleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
        dispdrvSelectReg8(0x10); // Enter Sleep Mode
    } else {
        dispdrvSelectReg8(0x11); // Sleep Out
    }

    SET(DISP_CS);
}

void ili9341SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvSelectReg8(0x2A); // Column Address Set
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2B); // Page Address Set
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2C); // Memory Write
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = ili9341Init,
    .sleep = ili9341Sleep,
    .setWindow = ili9341SetWindow,
    .rotate = ili9341Rotate,
    .shift = ili9341Shift,
};

#include "../dispdrv.h"

void ili9481Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence

    dispdrvSelectReg8(0x01);
    utilmDelay(120);

    dispdrvSelectReg8(0x11);
    utilmDelay(20);

    dispdrvSelectReg8(0xD0);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x41);
    dispdrvSendData8(0x18);

    dispdrvSelectReg8(0xD1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x10);

    dispdrvSelectReg8(0xD2);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x11);

    dispdrvSelectReg8(0xC0);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x3B);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xC5);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xC8);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0x33);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x16);
    dispdrvSendData8(0x44);
    dispdrvSendData8(0x36);
    dispdrvSendData8(0x77);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xF3);
    dispdrvSendData8(0x40);
    dispdrvSendData8(0x0A);

    dispdrvSelectReg8(0xF0);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0xF6);
    dispdrvSendData8(0x84);

    dispdrvSelectReg8(0xF7);
    dispdrvSendData8(0x80);

    dispdrvSelectReg8(0x21);

    dispdrvSelectReg8(0x51);
    dispdrvSendData8(0xff);

    dispdrvSelectReg8(0x36);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0x3A);
    dispdrvSendData8(0x55);
    utilmDelay(120);

    dispdrvSelectReg8(0x29);
    utilmDelay(120);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9481Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x0B);
    } else {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x08);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9481Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x28);    // Display OFF
    utilmDelay(100);
    dispdrvSelectReg8(0x10);

    SET(DISP_CS);
}

void ili9481Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x11);    // Display ON
    utilmDelay(100);
    dispdrvSelectReg8(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9481SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .init = ili9481Init,
    .sleep = ili9481Sleep,
    .wakeup = ili9481Wakeup,
    .setWindow = ili9481SetWindow,
    .rotate = ili9481Rotate,
};

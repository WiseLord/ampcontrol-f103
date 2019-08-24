#include "../dispdrv.h"

static uint8_t shiftX = 0;

void ili9327Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    dispdrvSelectReg8(0xE9);
    dispdrvSendData8(0x20);

    dispdrvSelectReg8(0x11);
    utilmDelay(100);

    dispdrvSelectReg8(0xD1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x71);
    dispdrvSendData8(0x19);

    dispdrvSelectReg8(0xD0);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0x36);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0x3A);
    dispdrvSendData8(0x05);

    dispdrvSelectReg8(0xC1);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x02);

    dispdrvSelectReg8(0xC0);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x02);

    dispdrvSelectReg8(0xC5);
    dispdrvSendData8(0x04);

    dispdrvSelectReg8(0xD2);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x44);

    dispdrvSelectReg8(0xC8);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x67);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x40);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x80);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x2A);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xEF);

    dispdrvSelectReg8(0x2B);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x8F);

    dispdrvSelectReg8(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9327Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        shiftX = 32;
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x0B);
    } else {
        shiftX = 0;
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x08);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9327Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x28);    // Display OFF
    utilmDelay(100);
    dispdrvSelectReg8(0x10);

    SET(DISP_CS);
}

void ili9327Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x11);    // Display ON
    utilmDelay(100);
    dispdrvSelectReg8(0x29);

    SET(DISP_CS);
}

void ili9327SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvSelectReg8(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    x += shiftX;
    dispdrvSelectReg8(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2C);
}

const DispDriver dispdrv = {
    .width = 400,
    .height = 240,
    .init = ili9327Init,
    .sleep = ili9327Sleep,
    .wakeup = ili9327Wakeup,
    .setWindow = ili9327SetWindow,
    .rotate = ili9327Rotate,
};

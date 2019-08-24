#include "../dispdrv.h"

void s6d04d1Init(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0xf4);
    dispdrvSendData8(0x59);
    dispdrvSendData8(0x59);
    dispdrvSendData8(0x52);
    dispdrvSendData8(0x52);
    dispdrvSendData8(0x11);

    dispdrvSelectReg8(0xf5);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0b);
    dispdrvSendData8(0xf0);
    dispdrvSendData8(0x00);
    utilmDelay(10);

    dispdrvSelectReg8(0xf3);
    dispdrvSendData8(0xff);
    dispdrvSendData8(0x2a);
    dispdrvSendData8(0x2a);
    dispdrvSendData8(0x0a);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x72);
    dispdrvSendData8(0x72);
    dispdrvSendData8(0x20);

    dispdrvSelectReg8(0x3a);
    dispdrvSendData8(0x55);

    dispdrvSelectReg8(0xf2);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x1a);
    dispdrvSendData8(0x1a);

    dispdrvSelectReg8(0xf6);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x88);
    dispdrvSendData8(0x10);

    dispdrvSelectReg8(0xf7);
    dispdrvSendData8(0x0d);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0e);
    dispdrvSendData8(0x1c);
    dispdrvSendData8(0x29);
    dispdrvSendData8(0x2d);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x0e);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x1e);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x22);

    dispdrvSelectReg8(0xf8);
    dispdrvSendData8(0x0d);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0e);
    dispdrvSendData8(0x1c);
    dispdrvSendData8(0x29);
    dispdrvSendData8(0x2d);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x0e);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x1e);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x22);

    dispdrvSelectReg8(0xf9);
    dispdrvSendData8(0x1e);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0a);
    dispdrvSendData8(0x19);
    dispdrvSendData8(0x23);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x3f);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x16);
    dispdrvSendData8(0x19);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x22);


    dispdrvSelectReg8(0xfA);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0x19);
    dispdrvSendData8(0x23);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x3f);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x16);
    dispdrvSendData8(0x19);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x22);

    dispdrvSelectReg8(0xfB);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x1C);
    dispdrvSendData8(0x29);
    dispdrvSendData8(0x2D);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x1E);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x22);

    dispdrvSelectReg8(0xfC);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x1C);
    dispdrvSendData8(0x29);
    dispdrvSendData8(0x2D);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x1E);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x22);

    dispdrvSelectReg8(0xFD);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0x36);
    dispdrvSendData8(0xC8);

    dispdrvSelectReg8(0x35);
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

    dispdrvSelectReg8(0xF1);
    dispdrvSendData8(0x5A);

    dispdrvSelectReg8(0xFF);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x40);

    dispdrvSelectReg8(0x11);
    utilmDelay(120);

    dispdrvSelectReg8(0xF1);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x29);
    utilmDelay(40);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void s6d04d1Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x08);
    } else {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0xC8);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void s6d04d1Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x28);    // Display OFF
    utilmDelay(100);
    dispdrvSelectReg8(0x10);

    SET(DISP_CS);
}

void s6d04d1Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x11);    // Display ON
    utilmDelay(100);
    dispdrvSelectReg8(0x29);

    SET(DISP_CS);
}

void s6d04d1SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .width = 400,
    .height = 240,
    .init = s6d04d1Init,
    .sleep = s6d04d1Sleep,
    .wakeup = s6d04d1Wakeup,
    .setWindow = s6d04d1SetWindow,
    .rotate = s6d04d1Rotate,
};

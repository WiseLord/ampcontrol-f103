#include "../dispdrv.h"

void ssd1283aInit(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x10);
    dispdrvSendData8(0x2F);
    dispdrvSendData8(0x8E);

    dispdrvSelectReg8(0x11);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0C);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x21);

    dispdrvSelectReg8(0x28);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x06);

    dispdrvSelectReg8(0x28);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x05);

    dispdrvSelectReg8(0x27);
    dispdrvSendData8(0x05);
    dispdrvSendData8(0x7F);

    dispdrvSelectReg8(0x29);
    dispdrvSendData8(0x89);
    dispdrvSendData8(0xA1);

    dispdrvSelectReg8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    utilmDelay(100);

    dispdrvSelectReg8(0x29);
    dispdrvSendData8(0x80);
    dispdrvSendData8(0xB0);

    utilmDelay(30);

    dispdrvSelectReg8(0x29);
    dispdrvSendData8(0xFF);
    dispdrvSendData8(0xFE);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x23);

    utilmDelay(30);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x33);

    dispdrvSelectReg8(0x01);
    dispdrvSendData8(0x23); //0x21
    dispdrvSendData8(0x83);

    dispdrvSelectReg8(0x03);
    dispdrvSendData8(0x68);
    dispdrvSendData8(0x30);

    dispdrvSelectReg8(0x2F);
    dispdrvSendData8(0xFF);
    dispdrvSendData8(0xFF);

    dispdrvSelectReg8(0x2C);
    dispdrvSendData8(0x80);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x27);
    dispdrvSendData8(0x05);
    dispdrvSendData8(0x70);

    dispdrvSelectReg8(0x02);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x0B);
    dispdrvSendData8(0x58);
    dispdrvSendData8(0x0C);

    dispdrvSelectReg8(0x12);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x09);

    dispdrvSelectReg8(0x13);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x00);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1283aSleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0x11);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1283aWakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    utilmDelay(20);

    dispdrvSelectReg8(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    utilmDelay(20);

    dispdrvSelectReg8(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    utilmDelay(20);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x33);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1283aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    uint8_t x0 = (uint8_t)x;
    uint8_t y0 = (uint8_t)y + 2;
    uint8_t x1 = (uint8_t)(x + w - 1);
    uint8_t y1 = (uint8_t)(y + h - 1) + 2;

    dispdrvSelectReg8(0x44);
    dispdrvSendData8(y1);
    dispdrvSendData8(y0);

    dispdrvSelectReg8(0x45);
    dispdrvSendData8(x1);
    dispdrvSendData8(x0);

    dispdrvSelectReg8(0x21);
    dispdrvSendData8(x0);
    dispdrvSendData8(y0);

    dispdrvSelectReg8(0x22);
}

const DispDriver dispdrv = {
    .width = 130,
    .height = 130,
    .init = ssd1283aInit,
//    .sleep = ssd1283aSleep,
//    .wakeup = ssd1283aWakeup,
    .setWindow = ssd1283aSetWindow,
};

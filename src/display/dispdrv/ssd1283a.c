#include "../dispdrv.h"

static bool _rotate = 0;

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

    DISP_MDELAY(100);

    dispdrvSelectReg8(0x29);
    dispdrvSendData8(0x80);
    dispdrvSendData8(0xB0);

    DISP_MDELAY(30);

    dispdrvSelectReg8(0x29);
    dispdrvSendData8(0xFF);
    dispdrvSendData8(0xFE);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x23);

    DISP_MDELAY(30);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x33);

    dispdrvSelectReg8(0x01);
    dispdrvSendData8(0x21);
    dispdrvSendData8(0x83);

    dispdrvSelectReg8(0x03);
    dispdrvSendData8(0x68);
    dispdrvSendData8(0x20);

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

void ssd1283aRotate(bool rotate)
{
    _rotate = rotate;

    CLR(DISP_CS);

    if (rotate) {
        dispdrvSelectReg8(0x01);
        dispdrvSendData8(0x23);
        dispdrvSendData8(0x83);

        dispdrvSelectReg8(0x03);
        dispdrvSendData8(0x68);
        dispdrvSendData8(0x30);
    } else {
        dispdrvSelectReg8(0x01);
        dispdrvSendData8(0x21);
        dispdrvSendData8(0x83);

        dispdrvSelectReg8(0x03);
        dispdrvSendData8(0x68);
        dispdrvSendData8(0x20);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1283aSleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
    } else {
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1283aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    bool rot = _rotate;
    const int16_t sh = 2;

    uint8_t x0 = (uint8_t)(x + (rot ? 0 : sh));
    uint8_t y0 = (uint8_t)(y - (rot ? 0 : sh));
    uint8_t x1 = (uint8_t)(x + (rot ? 0 : sh) + w - 1);
    uint8_t y1 = (uint8_t)(y - (rot ? 0 : sh) + h - 1);

    dispdrvSelectReg8(0x44);
    if (_rotate) {
        dispdrvSendData8(y1 + sh);
        dispdrvSendData8(y0 + sh);
    } else {
        dispdrvSendData8(dispdrv.height - 1 - y0);
        dispdrvSendData8(dispdrv.height - 1 - y1);
    }

    dispdrvSelectReg8(0x45);
    dispdrvSendData8(x1);
    dispdrvSendData8(x0);

    dispdrvSelectReg8(0x21);
    dispdrvSendData8(x0);
    if (_rotate) {
        dispdrvSendData8(y0 + sh);
    } else {
        dispdrvSendData8(dispdrv.height - 1 - y0);
    }

    dispdrvSelectReg8(0x22);
}

const DispDriver dispdrv = {
    .width = 130,
    .height = 130,
    .init = ssd1283aInit,
    .sleep = ssd1283aSleep,
    .setWindow = ssd1283aSetWindow,
    .rotate = ssd1283aRotate,
};

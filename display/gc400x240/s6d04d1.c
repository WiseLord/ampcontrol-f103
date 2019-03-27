#include "s6d04d1.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 400,
    .height = 240,
    .drawPixel = s6d04d1DrawPixel,
    .drawRectangle = s6d04d1DrawRectangle,
    .drawImage = s6d04d1DrawImage,
    .rotate = s6d04d1Rotate,
};

__attribute__((always_inline))
static inline void s6d04d1SelectReg(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void s6d04d1SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    s6d04d1SelectReg(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    s6d04d1SelectReg(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    s6d04d1SelectReg(0x2C);
}

static inline void s6d04d1InitSeq(void)
{
    CLR(DISP_CS);

    s6d04d1SelectReg(0xf4);
    dispdrvSendData8(0x59);
    dispdrvSendData8(0x59);
    dispdrvSendData8(0x52);
    dispdrvSendData8(0x52);
    dispdrvSendData8(0x11);

    s6d04d1SelectReg(0xf5);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0b);
    dispdrvSendData8(0xf0);
    dispdrvSendData8(0x00);
    LL_mDelay(10);

    s6d04d1SelectReg(0xf3);
    dispdrvSendData8(0xff);
    dispdrvSendData8(0x2a);
    dispdrvSendData8(0x2a);
    dispdrvSendData8(0x0a);
    dispdrvSendData8(0x22);
    dispdrvSendData8(0x72);
    dispdrvSendData8(0x72);
    dispdrvSendData8(0x20);

    s6d04d1SelectReg(0x3a);
    dispdrvSendData8(0x55);

    s6d04d1SelectReg(0xf2);
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

    s6d04d1SelectReg(0xf6);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x88);
    dispdrvSendData8(0x10);

    s6d04d1SelectReg(0xf7);
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

    s6d04d1SelectReg(0xf8);
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

    s6d04d1SelectReg(0xf9);
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


    s6d04d1SelectReg(0xfA);
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

    s6d04d1SelectReg(0xfB);
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

    s6d04d1SelectReg(0xfC);
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

    s6d04d1SelectReg(0xFD);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x01);

    s6d04d1SelectReg(0x36);
    dispdrvSendData8(0xC8);

    s6d04d1SelectReg(0x35);
    dispdrvSendData8(0x00);


    s6d04d1SelectReg(0x2A);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xEF);


    s6d04d1SelectReg(0x2B);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x8F);

    s6d04d1SelectReg(0xF1);
    dispdrvSendData8(0x5A);

    s6d04d1SelectReg(0xFF);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x40);

    s6d04d1SelectReg(0x11);
    LL_mDelay(120);

    s6d04d1SelectReg(0xF1);
    dispdrvSendData8(0x00);

    s6d04d1SelectReg(0x29);
    LL_mDelay(40);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void s6d04d1Init(DispDriver **driver)
{
    *driver = &drv;
    s6d04d1InitSeq();
}

void s6d04d1Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        s6d04d1SelectReg(0x36);
        dispdrvSendData8(0x08);
    } else {
        s6d04d1SelectReg(0x36);
        dispdrvSendData8(0xC8);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void s6d04d1Sleep(void)
{
    CLR(DISP_CS);

    s6d04d1SelectReg(0x28);    // Display OFF
    LL_mDelay(100);
    s6d04d1SelectReg(0x10);

    SET(DISP_CS);
}

void s6d04d1Wakeup(void)
{
    CLR(DISP_CS);

    s6d04d1SelectReg(0x11);    // Display ON
    LL_mDelay(100);
    s6d04d1SelectReg(0x29);

    SET(DISP_CS);
}

void s6d04d1DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    s6d04d1SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void s6d04d1DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    s6d04d1SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void s6d04d1DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    s6d04d1SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

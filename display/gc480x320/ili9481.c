#include "ili9481.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define ILI9481_WIDTH           320
#define ILI9481_HEIGHT          480
#define ILI9481_PIXELS          (ILI9481_WIDTH * ILI9481_HEIGHT)

static DispDriver drv = {
    .width = ILI9481_HEIGHT,
    .height = ILI9481_WIDTH,
    .drawPixel = ili9481DrawPixel,
    .drawRectangle = ili9481DrawRectangle,
    .drawImage = ili9481DrawImage,
};

static inline void ili9481SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void ili9481SelectReg(uint8_t reg)
{
    CLR(DISP_8BIT_RS);
    dispdrvSendData8(reg);
    SET(DISP_8BIT_RS);
}

static inline void ili9481InitSeq(void)
{
    // Wait for reset
    LL_mDelay(50);

    CLR(DISP_8BIT_CS);

    // Initial Sequence

    ili9481SelectReg(0x01);
    LL_mDelay(120);

    ili9481SelectReg(0x11);
    LL_mDelay(20);

    ili9481SelectReg(0xD0);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x41);
    dispdrvSendData8(0x18);

    ili9481SelectReg(0xD1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x10);

    ili9481SelectReg(0xD2);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x11);

    ili9481SelectReg(0xC0);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x3B);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x00);

    ili9481SelectReg(0xC5);
    dispdrvSendData8(0x00);

    ili9481SelectReg(0xC8);
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

    ili9481SelectReg(0xF3);
    dispdrvSendData8(0x40);
    dispdrvSendData8(0x0A);

    ili9481SelectReg(0xF0);
    dispdrvSendData8(0x08);

    ili9481SelectReg(0xF6);
    dispdrvSendData8(0x84);

    ili9481SelectReg(0xF7);
    dispdrvSendData8(0x80);

    ili9481SelectReg(0x21);

    ili9481SelectReg(0x51);
    dispdrvSendData8(0xff);

    ili9481SelectReg(0x36);
    dispdrvSendData8(0x08);

    ili9481SelectReg(0x3A);
    dispdrvSendData8(0x55);
    LL_mDelay(120);

    ili9481SelectReg(0x29);
    LL_mDelay(120);

    SET(DISP_8BIT_CS);
}

static inline void ili9481SetWindow(uint16_t x, uint16_t y, uint16_t w,
                                    uint16_t h) __attribute__((always_inline));
static inline void ili9481SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9481SelectReg(0x2A);
    dispdrvSendData16(y);
    dispdrvSendData16(y + h - 1);

    ili9481SelectReg(0x2B);
    dispdrvSendData16(x);
    dispdrvSendData16(x + w - 1);

    ili9481SelectReg(0x2C);
}

void ili9481Init(DispDriver **driver)
{
    *driver = &drv;

    SET(DISP_8BIT_LED);
    SET(DISP_8BIT_RD);
    SET(DISP_8BIT_WR);
    SET(DISP_8BIT_RS);
    SET(DISP_8BIT_CS);

    CLR(DISP_8BIT_RST);
    LL_mDelay(1);
    SET(DISP_8BIT_RST);

    ili9481InitSeq();
}

void ili9481Sleep(void)
{
    CLR(DISP_8BIT_CS);

    ili9481SelectReg(0x28);    // Display OFF
    LL_mDelay(100);
    ili9481SelectReg(0x10);

    SET(DISP_8BIT_CS);
}

void ili9481Wakeup(void)
{
    CLR(DISP_8BIT_CS);

    ili9481SelectReg(0x11);    // Display ON
    LL_mDelay(100);
    ili9481SelectReg(0x29);

    SET(DISP_8BIT_CS);
}

void ili9481DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    ili9481SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_8BIT_CS);
}

void ili9481DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    ili9481SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_8BIT_CS);
}

void ili9481DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_8BIT_CS);

    ili9481SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_8BIT_CS);
}

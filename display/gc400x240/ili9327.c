#include "ili9327.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define ILI9327_WIDTH           240
#define ILI9327_HEIGHT          400
#define ILI9327_PIXELS          (ILI9327_WIDTH * ILI9327_HEIGHT)

static DispDriver drv = {
    .width = ILI9327_HEIGHT,
    .height = ILI9327_WIDTH,
    .drawPixel = ili9327DrawPixel,
    .drawRectangle = ili9327DrawRectangle,
    .drawImage = ili9327DrawImage,
    .rotate = ili9327Rotate,
};

static uint8_t shiftX = 0;

static inline void ili9327SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void ili9327SelectReg(uint8_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    SET(DISP_RS);
}

static inline void ili9327InitSeq(void)
{
    // Wait for reset
    LL_mDelay(50);

    CLR(DISP_CS);

    // Initial Sequence
    ili9327SelectReg(0xE9);
    dispdrvSendData8(0x20);

    ili9327SelectReg(0x11);
    LL_mDelay(100);

    ili9327SelectReg(0xD1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x71);
    dispdrvSendData8(0x19);

    ili9327SelectReg(0xD0);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x08);

    ili9327SelectReg(0x36);
    dispdrvSendData8(0x08);

    ili9327SelectReg(0x3A);
    dispdrvSendData8(0x05);

    ili9327SelectReg(0xC1);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0x02);

    ili9327SelectReg(0xC0);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x02);

    ili9327SelectReg(0xC5);
    dispdrvSendData8(0x04);

    ili9327SelectReg(0xD2);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x44);

    ili9327SelectReg(0xC8);
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

    ili9327SelectReg(0x2A);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xEF);

    ili9327SelectReg(0x2B);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x8F);

    ili9327SelectReg(0x29);

    SET(DISP_CS);
}

static inline void ili9327SetWindow(uint16_t x, uint16_t y, uint16_t w,
                                    uint16_t h) __attribute__((always_inline));
static inline void ili9327SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9327SelectReg(0x2A);
    dispdrvSendData8(y >> 8);
    dispdrvSendData8(y & 0xFF);
    dispdrvSendData8((y + h - 1) >> 8);
    dispdrvSendData8((y + h - 1) & 0xFF);

    x += shiftX;
    ili9327SelectReg(0x2B);
    dispdrvSendData8(x >> 8);
    dispdrvSendData8(x & 0xFF);
    dispdrvSendData8((x + w - 1) >> 8);
    dispdrvSendData8((x + w - 1) & 0xFF);

    ili9327SelectReg(0x2C);
}

void ili9327Init(DispDriver **driver)
{
    *driver = &drv;
    ili9327InitSeq();
}

void ili9327Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        shiftX = 32;
        ili9327SelectReg(0x36);
        dispdrvSendData8(0x0B);
    } else {
        shiftX = 0;
        ili9327SelectReg(0x36);
        dispdrvSendData8(0x08);
    }

    SET(DISP_CS);
}

void ili9327Sleep(void)
{
    CLR(DISP_CS);

    ili9327SelectReg(0x28);    // Display OFF
    LL_mDelay(100);
    ili9327SelectReg(0x10);

    SET(DISP_CS);
}

void ili9327Wakeup(void)
{
    CLR(DISP_CS);

    ili9327SelectReg(0x11);    // Display ON
    LL_mDelay(100);
    ili9327SelectReg(0x29);

    SET(DISP_CS);
}

void ili9327DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9327SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void ili9327DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9327SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void ili9327DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_CS);

    ili9327SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

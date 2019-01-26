#include "lph9157.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 176,
    .height = 132,
    .drawPixel = lph9157DrawPixel,
    .drawRectangle = lph9157DrawRectangle,
    .drawImage = lph9157DrawImage,
};

__attribute__((always_inline))
static inline void lph9157SelectReg(uint8_t cmd)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(cmd);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

void lph9157SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    lph9157SelectReg(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    lph9157SelectReg(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    lph9157SelectReg(0x2C);
}

static void lph9157InitSeq(void)
{
    CLR(DISP_CS);

    lph9157SelectReg(0x01);
    lph9157SelectReg(0x11);
    LL_mDelay(20);
    lph9157SelectReg(0x3a);
    dispdrvSendData8(0x05);
    LL_mDelay(20);
    lph9157SelectReg(0x36);
    dispdrvSendData8(0x40);
    lph9157SelectReg(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157Init(DispDriver **driver)
{
    *driver = &drv;
    lph9157InitSeq();
}

void lph9157Sleep(void)
{
    CLR(DISP_CS);

    lph9157SelectReg(0x10);
    lph9157SelectReg(0x28);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157Wakeup(void)
{
    CLR(DISP_CS);

    lph9157SelectReg(0x11);
    lph9157SelectReg(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    lph9157SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    lph9157SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void lph9157DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    lph9157SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

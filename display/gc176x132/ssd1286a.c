#include "ssd1286a.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 176,
    .height = 132,
    .drawPixel = ssd1286aDrawPixel,
    .drawRectangle = ssd1286aDrawRectangle,
    .drawImage = ssd1286aDrawImage,
};

__attribute__((always_inline))
static inline void ssd1286aSelectReg(uint8_t cmd)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(cmd);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

__attribute__((always_inline))
static void inline ssd1286aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    uint8_t x0 = (uint8_t)x;
    uint8_t y0 = (uint8_t)y;
    uint8_t x1 = (uint8_t)(x + w - 1);
    uint8_t y1 = (uint8_t)(y + h - 1);

    ssd1286aSelectReg(0x44);
    dispdrvSendData8(y1);
    dispdrvSendData8(y0);

    ssd1286aSelectReg(0x47);
    dispdrvSendData8(x1);
    dispdrvSendData8(x0);

    ssd1286aSelectReg(0x21);
    dispdrvSendData8(x0);
    dispdrvSendData8(y0);

    ssd1286aSelectReg(0x22);
}

static void ssd1286aInitSeq(void)
{
    CLR(DISP_CS);

    ssd1286aSelectReg(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    ssd1286aSelectReg(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    ssd1286aSelectReg(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    ssd1286aSelectReg(0x28);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x06);

    ssd1286aSelectReg(0x02);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSelectReg(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    ssd1286aSelectReg(0x01);
    dispdrvSendData8(0x30);
    dispdrvSendData8(0xb0);

    ssd1286aSelectReg(0x03);
    dispdrvSendData8(0x78);
    dispdrvSendData8(0x30);

    ssd1286aSelectReg(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aInit(DispDriver **driver)
{
    *driver = &drv;
    ssd1286aInitSeq();
}

void ssd1286aSleep(void)
{
    CLR(DISP_CS);

    ssd1286aSelectReg(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSelectReg(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    ssd1286aSelectReg(0x11);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSelectReg(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aWakeup(void)
{
    CLR(DISP_CS);

    ssd1286aSelectReg(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    LL_mDelay(20);

    ssd1286aSelectReg(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    LL_mDelay(20);

    ssd1286aSelectReg(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    LL_mDelay(20);

    ssd1286aSelectReg(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ssd1286aSetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ssd1286aSetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    ssd1286aSetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

#include "lph9157.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define LPH9157_WIDTH           132
#define LPH9157_HEIGHT          176
#define LPH9157_PIXELS          (LPH9157_WIDTH * LPH9157_HEIGHT)

static DispDriver drv = {
    .width = LPH9157_HEIGHT,
    .height = LPH9157_WIDTH,
    .drawPixel = lph9157DrawPixel,
    .drawRectangle = lph9157DrawRectangle,
    .drawImage = lph9157DrawImage,
};

static inline void lph9157SendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void lph9157SendCmd(uint8_t cmd)
{
    dispdrvWaitOperation();
    CLR(DISP_RS);

    dispdrvSendData8(cmd);

    dispdrvWaitOperation();
    SET(DISP_RS);
}

static void lph9157InitSeq(void)
{
    LL_mDelay(50);

    CLR(DISP_CS);

    lph9157SendCmd(0x01);
    lph9157SendCmd(0x11);
    LL_mDelay(20);
    lph9157SendCmd(0x3a);
    dispdrvSendData8(0x05);
    LL_mDelay(20);
    lph9157SendCmd(0x36);
    dispdrvSendData8(0x40);
    lph9157SendCmd(0x29);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void lph9157SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    lph9157SendCmd(0x2a);
    dispdrvSendData8(y);
    dispdrvSendData8(y + h - 1);

    lph9157SendCmd(0x2b);
    dispdrvSendData8(x);
    dispdrvSendData8(x + w - 1);

    lph9157SendCmd(0x2C);
}

void lph9157Init(DispDriver **driver)
{
    *driver = &drv;

    SET(DISP_RST);

    // Init magic
    lph9157InitSeq();
    SET(DISP_BCKL);
}

void lph9157Sleep(void)
{
    CLR(DISP_CS);

    lph9157SendCmd(0x10);
    lph9157SendCmd(0x28);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void lph9157Wakeup(void)
{
    CLR(DISP_CS);

    lph9157SendCmd(0x11);
    lph9157SendCmd(0x29);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void lph9157DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    lph9157SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void lph9157DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_CS);

    lph9157SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void lph9157DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_CS);

    lph9157SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

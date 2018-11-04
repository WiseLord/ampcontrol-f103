#include "ssd1286a.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define SSD1286A_WIDTH           132
#define SSD1286A_HEIGHT          176
#define SSD1286A_PIXELS          (SSD1286A_WIDTH * SSD1286A_HEIGHT)

static DispDriver drv = {
    .width = SSD1286A_HEIGHT,
    .height = SSD1286A_WIDTH,
    .drawPixel = ssd1286aDrawPixel,
    .drawRectangle = ssd1286aDrawRectangle,
    .drawImage = ssd1286aDrawImage,
};

static inline void ssd1286aSendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void ssd1286aSendCmd(uint8_t cmd)
{
    dispdrvWaitOperation();
    CLR(DISP_RS);

    dispdrvSendData8(cmd);

    dispdrvWaitOperation();
    SET(DISP_RS);
}

static void ssd1286aInitSeq(void)
{
    LL_mDelay(50);

    CLR(DISP_CS);

    ssd1286aSendCmd(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    ssd1286aSendCmd(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    ssd1286aSendCmd(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    ssd1286aSendCmd(0x28);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x06);

    ssd1286aSendCmd(0x02);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSendCmd(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    ssd1286aSendCmd(0x01);
    dispdrvSendData8(0x30);
    dispdrvSendData8(0xb0);

    ssd1286aSendCmd(0x03);
    dispdrvSendData8(0x78);
    dispdrvSendData8(0x30);

    ssd1286aSendCmd(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    dispdrvWaitOperation();
    SET(DISP_CS);
}


void ssd1286aSetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ssd1286aSendCmd(0x44);
    dispdrvSendData8(y + h - 1);
    dispdrvSendData8(y);

    ssd1286aSendCmd(0x47);
    dispdrvSendData8(x + w - 1);
    dispdrvSendData8(x);

    ssd1286aSendCmd(0x21);
    dispdrvSendData8(x);
    dispdrvSendData8(y);

    ssd1286aSendCmd(0x22);
}

void ssd1286aInit(DispDriver **driver)
{
    *driver = &drv;

    CLR(DISP_RST);
    LL_mDelay(100);
    SET(DISP_RST);

    // Init magic
    ssd1286aInitSeq();
    SET(DISP_BCKL);
}

void ssd1286aSleep(void)
{
    CLR(DISP_CS);

    ssd1286aSendCmd(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSendCmd(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    ssd1286aSendCmd(0x11);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSendCmd(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ssd1286aWakeup(void)
{
    CLR(DISP_CS);

    ssd1286aSendCmd(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    LL_mDelay(20);

    ssd1286aSendCmd(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    LL_mDelay(20);

    ssd1286aSendCmd(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    LL_mDelay(20);

    ssd1286aSendCmd(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ssd1286aDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ssd1286aSetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ssd1286aDrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ssd1286aSetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ssd1286aDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_CS);

    ssd1286aSetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

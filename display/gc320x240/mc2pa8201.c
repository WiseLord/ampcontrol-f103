#include "mc2pa8201.h"

#include <stm32f1xx_ll_utils.h>
#include <stm32f1xx_ll_spi.h>
#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = mc2pa8201DrawPixel,
    .drawRectangle = mc2pa8201DrawRectangle,
    .drawImage = mc2pa8201DrawImage,
};

__attribute__((always_inline))
static inline void mc2pa8201SelectReg(uint8_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void mc2pa8201SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    mc2pa8201SelectReg(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    mc2pa8201SelectReg(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    mc2pa8201SelectReg(0x2C);
}

static inline void mc2pa8201InitSeq(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    // Wait for reset
    LL_mDelay(50);

    CLR(DISP_CS);

    mc2pa8201SelectReg(0x01);
    LL_mDelay(100);

    mc2pa8201SelectReg(0x11);
    LL_mDelay(100);

    mc2pa8201SelectReg(0x20);

    mc2pa8201SelectReg(0x26); //Set Default Gamma
    dispdrvSendData8(0x04);

    mc2pa8201SelectReg(0x3A);
    dispdrvSendData8(0x05);

    mc2pa8201SelectReg(0x2d);

    for (uint8_t r1 = 0; r1 < 32; r1++)
        dispdrvSendData8((uint8_t)(r1 << 3));
    for (uint8_t r2 = 0; r2 < 32; r2++)
        dispdrvSendData8((r2));
    for (uint8_t g1 = 0; g1 < 64; g1++)
        dispdrvSendData8((uint8_t)(g1 << 2));
    for (uint8_t b1 = 0; b1 < 32; b1++)
        dispdrvSendData8((uint8_t)(b1 << 3));
    for (uint8_t b2 = 0; b2 < 32; b2++)
        dispdrvSendData8(0);

//    mc2pa8201SelectReg(0x51);
//    dispdrvSendData8(0xff);

//    mc2pa8201SelectReg(0x53);
//    dispdrvSendData8(0x24);

    mc2pa8201SelectReg(0x36);
    dispdrvSendData8(0x80);

    mc2pa8201SelectReg(0x29);

    SET(DISP_CS);
}

void mc2pa8201Init(DispDriver **driver)
{
    *driver = &drv;
    mc2pa8201InitSeq();
}

void mc2pa8201Sleep(void)
{
    CLR(DISP_CS);

    mc2pa8201SelectReg(0x28);    // Display OFF
    LL_mDelay(100);
    mc2pa8201SelectReg(0x10);

    SET(DISP_CS);
}

void mc2pa8201Wakeup(void)
{
    CLR(DISP_CS);

    mc2pa8201SelectReg(0x11);    // Display OFF
    LL_mDelay(100);
    mc2pa8201SelectReg(0x29);

    SET(DISP_CS);
}

void mc2pa8201DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    mc2pa8201SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void mc2pa8201DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    mc2pa8201SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void mc2pa8201DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    mc2pa8201SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

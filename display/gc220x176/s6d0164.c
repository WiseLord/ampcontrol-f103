#include "s6d0164.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 220,
    .height = 176,
    .drawPixel = s6d0164DrawPixel,
    .drawRectangle = s6d0164DrawRectangle,
    .drawImage = s6d0164DrawImage,
};

__attribute__((always_inline))
static inline void s6d0164SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

static void s6d0164WriteReg(uint16_t reg, uint16_t data)
{
    s6d0164SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void s6d0164SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    s6d0164WriteReg(0x0036, (uint16_t)y1);
    s6d0164WriteReg(0x0037, (uint16_t)y);
    s6d0164WriteReg(0x0038, (uint16_t)x1);
    s6d0164WriteReg(0x0039, (uint16_t)x);

    // Set cursor
    s6d0164WriteReg(0x0020, (uint16_t)y);
    s6d0164WriteReg(0x0021, (uint16_t)x);

    // Select RAM mode
    s6d0164SelectReg(0x0022);
}

static inline void s6d0164InitSeq(void)
{
    CLR(DISP_CS);

    s6d0164WriteReg(0x0011, 0x001A);
    s6d0164WriteReg(0x0012, 0x3121);
    s6d0164WriteReg(0x0013, 0x006C);
    s6d0164WriteReg(0x0014, 0x4249);

    s6d0164WriteReg(0x0010, 0x0800);
    LL_mDelay(10);
    s6d0164WriteReg(0x0011, 0x011A);
    LL_mDelay(10);
    s6d0164WriteReg(0x0011, 0x031A);
    LL_mDelay(10);
    s6d0164WriteReg(0x0011, 0x071A);
    LL_mDelay(10);
    s6d0164WriteReg(0x0011, 0x0F1A);
    LL_mDelay(10);
    s6d0164WriteReg(0x0011, 0x0F3A);
    LL_mDelay(30);

    s6d0164WriteReg(0x0001, 0x001C);
    s6d0164WriteReg(0x0002, 0x0100);
    s6d0164WriteReg(0x0003, 0x1030);
    s6d0164WriteReg(0x0007, 0x0000);
    s6d0164WriteReg(0x0008, 0x0808);

    s6d0164WriteReg(0x0009, 0x0000);

    s6d0164WriteReg(0x000B, 0x1100);
    s6d0164WriteReg(0x000C, 0x0000);
    s6d0164WriteReg(0x000F, 0x1401);
    s6d0164WriteReg(0x0015, 0x0000);

    s6d0164WriteReg(0x0050, 0x0001);
    s6d0164WriteReg(0x0051, 0x020B);
    s6d0164WriteReg(0x0052, 0x0805);
    s6d0164WriteReg(0x0053, 0x0404);
    s6d0164WriteReg(0x0054, 0x0C0C);
    s6d0164WriteReg(0x0055, 0x000C);
    s6d0164WriteReg(0x0056, 0x0101);
    s6d0164WriteReg(0x0057, 0x0400);
    s6d0164WriteReg(0x0058, 0x1108);
    s6d0164WriteReg(0x0059, 0x050C);

    s6d0164WriteReg(0x0020, 0x0000);
    s6d0164WriteReg(0x0021, 0x0000);

    s6d0164WriteReg(0x0036, 0x00AF);
    s6d0164WriteReg(0x0037, 0x0000);
    s6d0164WriteReg(0x0038, 0x00DB);
    s6d0164WriteReg(0x0039, 0x0000);

    s6d0164WriteReg(0x000F, 0x0B01);
    s6d0164WriteReg(0x0007, 0x0016);
    LL_mDelay(10);
    s6d0164WriteReg(0x0007, 0x0017);

    SET(DISP_CS);
}

void s6d0164Init(DispDriver **driver)
{
    *driver = &drv;
    s6d0164InitSeq();
}

void s6d0164Sleep(void)
{
    CLR(DISP_CS);

    s6d0164WriteReg(0x0007, 0x0000);    // Display OFF
    LL_mDelay(50);
    s6d0164WriteReg(0x0010, 0x0A01);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void s6d0164Wakeup(void)
{
    CLR(DISP_CS);

    s6d0164WriteReg(0x0010, 0x0A00);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    LL_mDelay(50);
    s6d0164WriteReg(0x0007, 0x1017);    // 65K color and display ON

    SET(DISP_CS);
}

void s6d0164DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    s6d0164SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void s6d0164DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    s6d0164SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void s6d0164DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    s6d0164SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

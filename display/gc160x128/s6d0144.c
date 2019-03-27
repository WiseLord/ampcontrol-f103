#include "s6d0144.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 160,
    .height = 128,
    .drawPixel = s6d0144DrawPixel,
    .drawRectangle = s6d0144DrawRectangle,
    .drawImage = s6d0144DrawImage,
};

__attribute__((always_inline))
static inline void s6d0144SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

static void s6d0144WriteReg(uint16_t reg, uint16_t data)
{
    s6d0144SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void s6d0144SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    s6d0144WriteReg(0x0044, (uint16_t)(y | (y1 << 8)));
    s6d0144WriteReg(0x0045, (uint16_t)(x | (x1 << 8)));

    // Set cursor
    s6d0144WriteReg(0x0021, (uint16_t)(y | (x << 8)));

    // Select RAM mode
    s6d0144SelectReg(0x0022);
}

static inline void s6d0144InitSeq(void)
{
    CLR(DISP_CS);

    /* Initialize S6D0144 */
    LL_mDelay(1);
    s6d0144WriteReg(0xB4, 0x0010);
    s6d0144WriteReg(0x07, 0x0000);
    s6d0144WriteReg(0x13, 0x0000);
    LL_mDelay(1);
    s6d0144WriteReg(0x12, 0x0071);
    s6d0144WriteReg(0x13, 0x080B);
    s6d0144WriteReg(0x14, 0x78DD);
    s6d0144WriteReg(0x10, 0x1910);

    s6d0144WriteReg(0x01, 0x0314);
    s6d0144WriteReg(0x02, 0x0100);
    s6d0144WriteReg(0x03, 0x0030);
    s6d0144WriteReg(0x08, 0x0101);
    s6d0144WriteReg(0x0b, 0x0005);
    s6d0144WriteReg(0x0C, 0x0002);
    LL_mDelay(10);


    s6d0144WriteReg(0x30, 0x0000);
    s6d0144WriteReg(0x31, 0x0506);
    s6d0144WriteReg(0x32, 0x0403);
    s6d0144WriteReg(0x33, 0x0200);
    s6d0144WriteReg(0x34, 0x0303);
    s6d0144WriteReg(0x35, 0x0002);
    s6d0144WriteReg(0x36, 0x0707);
    s6d0144WriteReg(0x37, 0x0200);
    s6d0144WriteReg(0x38, 0x0900);
    s6d0144WriteReg(0x11, 0x000A);

    s6d0144WriteReg(0x40, 0x0000);
    s6d0144WriteReg(0x41, 0x00E5);
    s6d0144WriteReg(0x42, 0x9F00);
    s6d0144WriteReg(0x43, 0x9F00);
    s6d0144WriteReg(0x44, 0x7F00);
    s6d0144WriteReg(0x45, 0x9f00);

    s6d0144WriteReg(0x13, 0x081B);
    LL_mDelay(50);
    s6d0144WriteReg(0x07, 0x0037);      // DTE =1
    LL_mDelay(20);

    SET(DISP_CS);
}

void s6d0144Init(DispDriver **driver)
{
    *driver = &drv;
    s6d0144InitSeq();
}

void s6d0144Sleep(void)
{
    CLR(DISP_CS);

    s6d0144WriteReg(0x0007, 0x0000);    // Display OFF
    LL_mDelay(50);
    s6d0144WriteReg(0x0010, 0x0A01);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void s6d0144Wakeup(void)
{
    CLR(DISP_CS);

    s6d0144WriteReg(0x0010, 0x0A00);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    LL_mDelay(50);
    s6d0144WriteReg(0x0007, 0x1017);    // 65K color and display ON

    SET(DISP_CS);
}

void s6d0144DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    s6d0144SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void s6d0144DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    s6d0144SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void s6d0144DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    s6d0144SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

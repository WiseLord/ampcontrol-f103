#include "lgdp4524.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 220,
    .height = 176,
    .drawPixel = lgdp4524DrawPixel,
    .drawRectangle = lgdp4524DrawRectangle,
    .drawImage = lgdp4524DrawImage,
};

__attribute__((always_inline))
static inline void lgdp4524SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

static void lgdp4524WriteReg(uint16_t reg, uint16_t data)
{
    lgdp4524SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void lgdp4524SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    lgdp4524WriteReg(0x0044, (uint16_t)(y | (y1 << 8)));
    lgdp4524WriteReg(0x0045, (uint16_t)(x | (x1 << 8)));

    // Set cursor
    lgdp4524WriteReg(0x0021, (uint16_t)(y | (x << 8)));

    // Select RAM mode
    lgdp4524SelectReg(0x0022);
}

static inline void lgdp4524InitSeq(void)
{
    CLR(DISP_CS);

    lgdp4524WriteReg(0x0007, 0x0000);
    lgdp4524WriteReg(0x0012, 0x0000);
    lgdp4524WriteReg(0x0013, 0x0000);
    lgdp4524WriteReg(0x0014, 0x0331);

    LL_mDelay(15); // Delay 15ms

    lgdp4524WriteReg(0x0012, 0x0009);
    lgdp4524WriteReg(0x0011, 0x0000);
    lgdp4524WriteReg(0x0010, 0x0104);
    lgdp4524WriteReg(0x0013, 0x0C31);
    lgdp4524WriteReg(0x0010, 0x0144);
    lgdp4524WriteReg(0x0011, 0x0200);
    lgdp4524WriteReg(0x0012, 0x0019);

    LL_mDelay(20); // Delay 20ms

    lgdp4524WriteReg(0x0013, 0x2C31);
    lgdp4524WriteReg(0x0010, 0x0140);

    LL_mDelay(20); // Delay 20ms

    lgdp4524WriteReg(0x0060, 0x2000);
    lgdp4524WriteReg(0x0060, 0x0000);
    lgdp4524WriteReg(0x0061, 0x0002);
    lgdp4524WriteReg(0x0001, 0x031B);
    lgdp4524WriteReg(0x0002, 0x0700);
    lgdp4524WriteReg(0x0003, 0x1030);
    lgdp4524WriteReg(0x0008, 0x0304);
    lgdp4524WriteReg(0x0009, 0x0000);
    lgdp4524WriteReg(0x000B, 0x5D2C);
    lgdp4524WriteReg(0x0021, 0x0000);
    lgdp4524WriteReg(0x0030, 0x0101);
    lgdp4524WriteReg(0x0031, 0x0604);
    lgdp4524WriteReg(0x0032, 0x0007);
    lgdp4524WriteReg(0x0033, 0x0403);
    lgdp4524WriteReg(0x0034, 0x0007);
    lgdp4524WriteReg(0x0035, 0x0402);
    lgdp4524WriteReg(0x0036, 0x0606);
    lgdp4524WriteReg(0x0037, 0x0304);
    lgdp4524WriteReg(0x0038, 0x070E);
    lgdp4524WriteReg(0x0039, 0x070E);
    lgdp4524WriteReg(0x003A, 0x0101);
    lgdp4524WriteReg(0x003B, 0x0101);
    lgdp4524WriteReg(0x003C, 0x0101);
    lgdp4524WriteReg(0x003D, 0x0101);
    lgdp4524WriteReg(0x003E, 0x0101);
    lgdp4524WriteReg(0x003F, 0x0101);

    lgdp4524WriteReg(0x0042, 0xDB00);
    lgdp4524WriteReg(0x0044, 0xAF00);
    lgdp4524WriteReg(0x0045, 0xDB00);
    lgdp4524WriteReg(0x0071, 0x0040);
    lgdp4524WriteReg(0x0072, 0x0002);

    LL_mDelay(10); // Delay 10ms

    lgdp4524WriteReg(0x0010, 0x3250);
    lgdp4524WriteReg(0x0007, 0x0001);

    LL_mDelay(30); // Delay 30ms

    lgdp4524WriteReg(0x0007, 0x0021);
    lgdp4524WriteReg(0x0007, 0x0023);

    LL_mDelay(30); // Delay 30ms

    lgdp4524WriteReg(0x0007, 0x0037);  // 65K color and display ON*/

    SET(DISP_CS);
}

void lgdp4524Init(DispDriver **driver)
{
    *driver = &drv;
    lgdp4524InitSeq();
}

void lgdp4524Sleep(void)
{
    CLR(DISP_CS);


    lgdp4524WriteReg(0x0007, 0x0036);

    LL_mDelay(20);

    lgdp4524WriteReg(0x0007, 0x0026);

    LL_mDelay(20);

    lgdp4524WriteReg(0x0007, 0x0000);

    LL_mDelay(10);

    lgdp4524WriteReg(0x0010, 0x0004);
    lgdp4524WriteReg(0x0012, 0x0000);
    lgdp4524WriteReg(0x0013, 0x0000);

    LL_mDelay(10);

    lgdp4524WriteReg(0x0010, 0x0008);

    SET(DISP_CS);
}

void lgdp4524Wakeup(void)
{
    CLR(DISP_CS);

    lgdp4524WriteReg(0x0000, 0x0001);

    LL_mDelay(10); // Delay 10ms

    lgdp4524WriteReg(0x0010, 0x0000);
    lgdp4524WriteReg(0x0014, 0x0331);

    LL_mDelay(15); // Delay 15ms

    lgdp4524WriteReg(0x0012, 0x0009);
    lgdp4524WriteReg(0x0011, 0x0000);
    lgdp4524WriteReg(0x0010, 0x0104);
    lgdp4524WriteReg(0x0013, 0x0C31);
    lgdp4524WriteReg(0x0010, 0x0144);
    lgdp4524WriteReg(0x0011, 0x0200);
    lgdp4524WriteReg(0x0012, 0x0019);

    LL_mDelay(20); // Delay 20ms

    lgdp4524WriteReg(0x0013, 0x2C31);
    lgdp4524WriteReg(0x0010, 0x0140);

    LL_mDelay(20); // Delay 20ms

    lgdp4524WriteReg(0x0010, 0x3250);
    lgdp4524WriteReg(0x0007, 0x0001);

    LL_mDelay(30); // Delay 30ms

    lgdp4524WriteReg(0x0007, 0x0021);
    lgdp4524WriteReg(0x0007, 0x0023);

    LL_mDelay(30); // Delay 30ms

    lgdp4524WriteReg(0x0007, 0x0037);  // 65K color and display ON*/

    SET(DISP_CS);
}

void lgdp4524DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    lgdp4524SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void lgdp4524DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    lgdp4524SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void lgdp4524DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    lgdp4524SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

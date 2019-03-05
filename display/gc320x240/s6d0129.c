#include "s6d0129.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = s6d0129DrawPixel,
    .drawRectangle = s6d0129DrawRectangle,
    .drawImage = s6d0129DrawImage,
    .rotate = s6d0129Rotate,
};

__attribute__((always_inline))
static inline void s6d0129SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void s6d0129WriteReg(uint16_t reg, uint16_t data)
{
    s6d0129SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void s6d0129SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    s6d0129WriteReg(0x0046, (uint16_t)(y | (y1 << 8)));
    s6d0129WriteReg(0x0048, (uint16_t)x);
    s6d0129WriteReg(0x0047, (uint16_t)x1);

    s6d0129WriteReg(0x0020, (uint16_t)y);
    s6d0129WriteReg(0x0021, (uint16_t)x);

    s6d0129SelectReg(0x0022);
}

static inline void s6d0129InitSeq(void)
{
    CLR(DISP_CS);

    s6d0129WriteReg(0x00e5, 0x8000);

    s6d0129WriteReg(0x0000, 0x0001);
    LL_mDelay(100);

    s6d0129WriteReg(0x0011, 0x2E00);
    s6d0129WriteReg(0x0014, 0x040B);
    s6d0129WriteReg(0x0010, 0x1040);
    LL_mDelay(10);
    s6d0129WriteReg(0x0013, 0x0040);
    LL_mDelay(10);
    s6d0129WriteReg(0x0013, 0x0060);
    LL_mDelay(10);
    s6d0129WriteReg(0x0013, 0x0070);
    LL_mDelay(6);
    s6d0129WriteReg(0x0011, 0x3704);
    s6d0129WriteReg(0x0010, 0x1600);
    LL_mDelay(2);
    s6d0129WriteReg(0x0001, 0x0B27);
    s6d0129WriteReg(0x0002, 0x0700);
    s6d0129WriteReg(0x0003, 0x1030);
    s6d0129WriteReg(0x0007, 0x0004);
    s6d0129WriteReg(0x0008, 0x0505);
    s6d0129WriteReg(0x0009, 0x0000);
    s6d0129WriteReg(0x000B, 0x0000);
    s6d0129WriteReg(0x000C, 0x0000);


    s6d0129WriteReg(0x0040, 0x0000);
//    s6d0129WriteReg(0x0042,0x013F);
//    s6d0129WriteReg(0x0043,0x0000);
//    s6d0129WriteReg(0x0044,0x013F);
//    s6d0129WriteReg(0x0045,0x0000);
    s6d0129WriteReg(0x0046, 0xEF00);
    s6d0129WriteReg(0x0047, 0x013F);
    s6d0129WriteReg(0x0048, 0x0000);
    LL_mDelay(5);

    s6d0129WriteReg(0x0030, 0x0000);
    s6d0129WriteReg(0x0031, 0x0006);
    s6d0129WriteReg(0x0032, 0x0000);
    s6d0129WriteReg(0x0033, 0x0000);
    s6d0129WriteReg(0x0034, 0x0707);
    s6d0129WriteReg(0x0035, 0x0700);
    s6d0129WriteReg(0x0036, 0x0303);
    s6d0129WriteReg(0x0037, 0x0007);
    s6d0129WriteReg(0x0038, 0x1100);
    s6d0129WriteReg(0x0039, 0x1100);

    s6d0129WriteReg(0x0007, 0x0015);
    LL_mDelay(5);
    s6d0129WriteReg(0x0007, 0x0017);
    LL_mDelay(5);

    SET(DISP_CS);
}

void s6d0129Init(DispDriver **driver)
{
    *driver = &drv;
    s6d0129InitSeq();
}

void s6d0129Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        s6d0129WriteReg(0x0001, 0x0827);
    } else {
        s6d0129WriteReg(0x0001, 0x0B27);
    }

    SET(DISP_CS);
}

void s6d0129Sleep(void)
{
    CLR(DISP_CS);

    s6d0129WriteReg(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    s6d0129WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    s6d0129WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    s6d0129WriteReg(0x0013, 0x0000);    // VREG1OUT voltage

    s6d0129WriteReg(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    LL_mDelay(200);
    s6d0129WriteReg(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void s6d0129Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    s6d0129WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    s6d0129WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    s6d0129WriteReg(0x0013, 0x0000);    // VREG1OUT voltage
    s6d0129WriteReg(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    LL_mDelay(200);
    s6d0129WriteReg(0x0007, 0x0000);    // Display control1
    s6d0129WriteReg(0x0013, 0x0000);    // Power control4 setting
    s6d0129WriteReg(0x0011, 0x2604);    // Power control2 setting
    s6d0129WriteReg(0x0014, 0x0015);    // Power control5 setting
    s6d0129WriteReg(0x0010, 0x3C00);    // Power control1 setting
    s6d0129WriteReg(0x0013, 0x0040);    // Power control4 setting
    LL_mDelay(10);
    s6d0129WriteReg(0x0013, 0x0060);    // Power control4 setting
    LL_mDelay(50);
    s6d0129WriteReg(0x0013, 0x0070);    // Power control4 setting
    LL_mDelay(40);
    s6d0129WriteReg(0x0007, 0x0017);    // 262K color and display ON

    SET(DISP_CS);
}

void s6d0129DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    s6d0129SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void s6d0129DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    s6d0129SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void s6d0129DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    s6d0129SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

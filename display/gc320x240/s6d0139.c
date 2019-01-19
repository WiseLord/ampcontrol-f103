#include "s6d0139.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = s6d0139DrawPixel,
    .drawRectangle = s6d0139DrawRectangle,
    .drawImage = s6d0139DrawImage,
};

__attribute__((always_inline))
static inline void s6d0139SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void s6d0139WriteReg(uint16_t reg, uint16_t data)
{
    s6d0139SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void s6d0139SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = drv.height - y - 1;   // TODO: rework it

    s6d0139WriteReg(0x0046, (uint16_t)((y1 << 8) + (y1 - h + 1)));
    s6d0139WriteReg(0x0048, (uint16_t)x);
    s6d0139WriteReg(0x0047, (uint16_t)x1);

    s6d0139WriteReg(0x0020, (uint16_t)y1);
    s6d0139WriteReg(0x0021, (uint16_t)x);

    s6d0139SelectReg(0x0022);
}

static inline void s6d0139InitSeq(void)
{
    CLR(DISP_CS);

    s6d0139WriteReg(0x0000, 0x0001);    // Start Oscillation
    LL_mDelay(10);
    s6d0139WriteReg(0x0007, 0x0000);    // Display control1
    s6d0139WriteReg(0x0013, 0x0000);    // Power control4 setting
    s6d0139WriteReg(0x0011, 0x2604);    // Power control2 setting
    s6d0139WriteReg(0x0014, 0x0015);    // Power control5 setting
    s6d0139WriteReg(0x0010, 0x3C00);    // Power control1 setting
    s6d0139WriteReg(0x0013, 0x0040);    // Power control4 setting
    LL_mDelay(10);
    s6d0139WriteReg(0x0013, 0x0060);    // Power control4 setting
    LL_mDelay(50);
    s6d0139WriteReg(0x0013, 0x0070);    // Power control4 setting
    LL_mDelay(40);

    s6d0139WriteReg(0x0001, 0x0127);    // Driver output setting (240x320 mode, GS=0, SS=1)
    s6d0139WriteReg(0x0002, 0x0700);    // LCD driving waveform setting
    s6d0139WriteReg(0x0003, 0x1020);    // Entry mode setting (TRI=0, DFM=0, BGR=1, ID1:ID0 =11)
    s6d0139WriteReg(0x0007, 0x0000);    // Display control1
    s6d0139WriteReg(0x0008, 0x0202);    // Display control2
    s6d0139WriteReg(0x000B, 0x0200);    // Frame cycle setting
    s6d0139WriteReg(0x000C, 0x0000);    // External display interface setting
    s6d0139WriteReg(0x0015, 0x0000);

    //gamma setting
    s6d0139WriteReg(0x0030, 0x0000);    // Gamma control1
    s6d0139WriteReg(0x0031, 0x0606);    // Gamma control2
    s6d0139WriteReg(0x0032, 0x0006);    // Gamma control3
    s6d0139WriteReg(0x0033, 0x0403);    // Gamma control4
    s6d0139WriteReg(0x0034, 0x0107);    // Gamma control5
    s6d0139WriteReg(0x0035, 0x0101);    // Gamma control6
    s6d0139WriteReg(0x0036, 0x0707);    // Gamma control7
    s6d0139WriteReg(0x0037, 0x0304);    // Gamma control8
    s6d0139WriteReg(0x0038, 0x0A00);    // Gamma control9
    s6d0139WriteReg(0x0039, 0x0706);    // Gamma control10

    s6d0139WriteReg(0x0040, 0x0000);    // Gate scan position (start G1 scan)
    s6d0139WriteReg(0x0041, 0x0000);    // Vertical scroll setting
    s6d0139WriteReg(0x0042, 0x013F);    // Screen end position (320-1)
    s6d0139WriteReg(0x0043, 0x0000);    // Screen start position (0)
    s6d0139WriteReg(0x0044, 0x0000);    // 2th Screen end position
    s6d0139WriteReg(0x0045, 0x0000);    // 2th Screen start position
    s6d0139WriteReg(0x0046, 0xEF00);    // window addr set for x0,x1 (0, 239)
    s6d0139WriteReg(0x0047, 0x013F);    // window addr set for y0    (319)
    s6d0139WriteReg(0x0048, 0x0000);    // window addr set for y1    (0)
    s6d0139WriteReg(0x0007, 0x0011);    // Display control1
    LL_mDelay(40);
    s6d0139WriteReg(0x0007, 0x0017);    // Display control1

    SET(DISP_CS);
}

void s6d0139Init(DispDriver **driver)
{
    *driver = &drv;
    s6d0139InitSeq();
}

void s6d0139Sleep(void)
{
    CLR(DISP_CS);

    s6d0139WriteReg(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    s6d0139WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    s6d0139WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    s6d0139WriteReg(0x0013, 0x0000);    // VREG1OUT voltage

    s6d0139WriteReg(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    LL_mDelay(200);
    s6d0139WriteReg(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void s6d0139Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    s6d0139WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    s6d0139WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    s6d0139WriteReg(0x0013, 0x0000);    // VREG1OUT voltage
    s6d0139WriteReg(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    LL_mDelay(200);
    s6d0139WriteReg(0x0007, 0x0000);    // Display control1
    s6d0139WriteReg(0x0013, 0x0000);    // Power control4 setting
    s6d0139WriteReg(0x0011, 0x2604);    // Power control2 setting
    s6d0139WriteReg(0x0014, 0x0015);    // Power control5 setting
    s6d0139WriteReg(0x0010, 0x3C00);    // Power control1 setting
    s6d0139WriteReg(0x0013, 0x0040);    // Power control4 setting
    LL_mDelay(10);
    s6d0139WriteReg(0x0013, 0x0060);    // Power control4 setting
    LL_mDelay(50);
    s6d0139WriteReg(0x0013, 0x0070);    // Power control4 setting
    LL_mDelay(40);
    s6d0139WriteReg(0x0007, 0x0017);    // 262K color and display ON

    SET(DISP_CS);
}

void s6d0139DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    s6d0139SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void s6d0139DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    s6d0139SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void s6d0139DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    s6d0139SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

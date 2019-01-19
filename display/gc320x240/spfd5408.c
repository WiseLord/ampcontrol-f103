#include "spfd5408.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = spfd5408DrawPixel,
    .drawRectangle = spfd5408DrawRectangle,
    .drawImage = spfd5408DrawImage,
};

__attribute__((always_inline))
static inline void spfd5408SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void spfd5408WriteReg(uint16_t reg, uint16_t data)
{
    spfd5408SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void spfd5408SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    spfd5408WriteReg(0x0050, (uint16_t)y);
    spfd5408WriteReg(0x0051, (uint16_t)y1);
    spfd5408WriteReg(0x0052, (uint16_t)x);
    spfd5408WriteReg(0x0053, (uint16_t)x1);

    // Set cursor
    spfd5408WriteReg(0x0020, (uint16_t)y);
    spfd5408WriteReg(0x0021, (uint16_t)x);

    // Set RAM mode
    spfd5408SelectReg(0x0022);
}

static inline void spfd5408InitSeq(void)
{
    // Wait for reset
    LL_mDelay(50);

    CLR(DISP_CS);
    // Initial Sequence
    spfd5408WriteReg(0x0000, 0x0000);
    spfd5408WriteReg(0x0001, 0x0100);
    spfd5408WriteReg(0x0002, 0x0700);
    spfd5408WriteReg(0x0003, 0x1030);
    spfd5408WriteReg(0x0004, 0x0000);
    spfd5408WriteReg(0x0008, 0x0207);
    spfd5408WriteReg(0x0009, 0x0000);
    spfd5408WriteReg(0x000A, 0x0000);
    spfd5408WriteReg(0x000C, 0x0000);
    spfd5408WriteReg(0x000D, 0x0000);
    spfd5408WriteReg(0x000F, 0x0000);
    spfd5408WriteReg(0x0007, 0x0101);

    // Power On Sequence
    spfd5408WriteReg(0x0010, 0x14B0);
    LL_mDelay(20);
    spfd5408WriteReg(0x0011, 0x0007);
    LL_mDelay(20);
    spfd5408WriteReg(0x0017, 0x0001);
    LL_mDelay(20);
    spfd5408WriteReg(0x0012, 0x01B8);
    LL_mDelay(20);
    spfd5408WriteReg(0x0013, 0x1300);
    LL_mDelay(20);
    spfd5408WriteReg(0x0029, 0x000F);

    // Adjust the Gamma Curve
    spfd5408WriteReg(0x0030, 0x0102);
    spfd5408WriteReg(0x0031, 0x0C18);
    spfd5408WriteReg(0x0032, 0x0F1C);
    spfd5408WriteReg(0x0033, 0x3617);
    spfd5408WriteReg(0x0034, 0x350B);
    spfd5408WriteReg(0x0035, 0x1004);
    spfd5408WriteReg(0x0036, 0x0D06);
    spfd5408WriteReg(0x0037, 0x0318);
    spfd5408WriteReg(0x0038, 0x0604);
    spfd5408WriteReg(0x0039, 0x0005);
    spfd5408WriteReg(0x003A, 0x0A05);
    spfd5408WriteReg(0x003B, 0x0A00);
    spfd5408WriteReg(0x003C, 0x020F);
    spfd5408WriteReg(0x003D, 0x050F);
    spfd5408WriteReg(0x003E, 0x0303);
    spfd5408WriteReg(0x003F, 0x0505);


    // Set GRAM area
    spfd5408WriteReg(0x0020, 0x0000);   // GRAM horisontal address
    spfd5408WriteReg(0x0021, 0x0000);   // GRAM vertical address

    spfd5408WriteReg(0x0050, 0x0000);   // Horizontal GRAM Start Address
    spfd5408WriteReg(0x0051, 0x00EF);   // Horizontal GRAM End Address
    spfd5408WriteReg(0x0052, 0x0000);   // Vertical GRAM Start Address
    spfd5408WriteReg(0x0053, 0x013F);   // Vertical GRAM End Address

    spfd5408WriteReg(0x0060, 0x2700);   // Gate scan line
    spfd5408WriteReg(0x0061, 0x0003);   // NDV, VLE, REV
    spfd5408WriteReg(0x006A, 0x0000);   // Set scrolling line

    // Partial Display Control
    spfd5408WriteReg(0x0080, 0x0000);
    spfd5408WriteReg(0x0081, 0x0000);
    spfd5408WriteReg(0x0082, 0x0000);
    spfd5408WriteReg(0x0083, 0x0000);
    spfd5408WriteReg(0x0084, 0x0000);
    spfd5408WriteReg(0x0085, 0x0000);

    // Panel Control
    spfd5408WriteReg(0x0090, 0x0018);
    spfd5408WriteReg(0x0092, 0x0000);
    spfd5408WriteReg(0x0093, 0x0103);
    spfd5408WriteReg(0x0095, 0x0110);
    spfd5408WriteReg(0x0097, 0x0000);
    spfd5408WriteReg(0x0098, 0x0000);

    spfd5408WriteReg(0x00F0, 0x5408);
    spfd5408WriteReg(0x00F2, 0x00DF);
    spfd5408WriteReg(0x00F3, 0x0006);
    spfd5408WriteReg(0x00F4, 0x001F);
    spfd5408WriteReg(0x00F7, 0x0008);
    spfd5408WriteReg(0x00F0, 0x0000);

    spfd5408WriteReg(0x0007, 0x0173);   // 262K color and display ON
    LL_mDelay(150);


    SET(DISP_CS);
}

void spfd5408Init(DispDriver **driver)
{
    *driver = &drv;
    spfd5408InitSeq();
}

void spfd5408Sleep(void)
{
    spfd5408WriteReg(0x0007, 0x0000);   // Display OFF
    // Power Off Sequence
    spfd5408WriteReg(0x0010, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0011, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0017, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0012, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0013, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0010, 0x0002);   // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void spfd5408Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    spfd5408WriteReg(0x0010, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0011, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0017, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0012, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0013, 0x0000);
    LL_mDelay(20);
    spfd5408WriteReg(0x0010, 0x14B0);
    LL_mDelay(20);
    spfd5408WriteReg(0x0011, 0x0007);
    LL_mDelay(20);
    spfd5408WriteReg(0x0017, 0x0001);
    LL_mDelay(20);
    spfd5408WriteReg(0x0012, 0x01B8);
    LL_mDelay(20);
    spfd5408WriteReg(0x0013, 0x1300);
    LL_mDelay(20);
    spfd5408WriteReg(0x0029, 0x000F);

    spfd5408WriteReg(0x0007, 0x0173);   // 262K color and display ON

    SET(DISP_CS);
}

void spfd5408DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    spfd5408SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void spfd5408DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    spfd5408SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void spfd5408DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    spfd5408SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

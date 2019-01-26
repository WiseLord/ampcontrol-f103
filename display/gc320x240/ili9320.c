#include "ili9320.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = ili9320DrawPixel,
    .drawRectangle = ili9320DrawRectangle,
    .drawImage = ili9320DrawImage,
    .rotate = ili9320Rotate,
    .shift = ili9320Shift,
};

__attribute__((always_inline))
static inline void ili9320SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void ili9320WriteReg(uint16_t reg, uint16_t value)
{
    ili9320SelectReg(reg);
    dispdrvSendData16(value);
}

static inline void ili9320SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    ili9320WriteReg(0x0050, (uint16_t)y);
    ili9320WriteReg(0x0051, (uint16_t)y1);
    ili9320WriteReg(0x0052, (uint16_t)x);
    ili9320WriteReg(0x0053, (uint16_t)x1);

    // Set cursor
    ili9320WriteReg(0x0020, (uint16_t)y);
    ili9320WriteReg(0x0021, (uint16_t)x);

    // Select RAM mode
    ili9320SelectReg(0x0022);
}

static inline void ili9320InitSeq(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    ili9320WriteReg(0x00E5, 0x8000);    // Set the Vcore voltage and this setting is must
    ili9320WriteReg(0x0000, 0x0001);    // Start internal OSC
    ili9320WriteReg(0x0001, 0x0100);    // Set SS and SM bit
    ili9320WriteReg(0x0002, 0x0700);    // Set 1 line inversion
    ili9320WriteReg(0x0003, 0x1030);    // Set GRAM write direction and BGR = 1
    ili9320WriteReg(0x0004, 0x0000);    // Resize register

    ili9320WriteReg(0x0008, 0x0202);    // Set the back porch and front porch
    ili9320WriteReg(0x0009, 0x0000);    // Set non-display area refresh cycle ICS[3:0]
    ili9320WriteReg(0x000A, 0x0000);    // FMARK function
    ili9320WriteReg(0x000C, 0x0000);    // RGB interface setting
    ili9320WriteReg(0x000D, 0x0000);    // Frame marker position
    ili9320WriteReg(0x000F, 0x0000);    // RGB interface polarity

    // Power On Sequence
    ili9320WriteReg(0x0010, 0x17B0);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0037);    // DC1[2:0], DC0[2:0], VC[2:0]
    ili9320WriteReg(0x0012, 0x013A);    // VREG1OUT voltage
    ili9320WriteReg(0x0013, 0x1600);    // VDV[4:0] for VCOM amplitude
    ili9320WriteReg(0x0029, 0x000C);    // VCM[4:0] for VCOMH

    // Adjust the Gamma Curve
    ili9320WriteReg(0x0030, 0x0007);
    ili9320WriteReg(0x0031, 0x0700);
    ili9320WriteReg(0x0032, 0x0707);
    ili9320WriteReg(0x0035, 0x0007);
    ili9320WriteReg(0x0036, 0x000F);
    ili9320WriteReg(0x0037, 0x0700);
    ili9320WriteReg(0x0038, 0x0707);
    ili9320WriteReg(0x0039, 0x0007);
    ili9320WriteReg(0x003C, 0x0700);
    ili9320WriteReg(0x003D, 0x0F00);

    // Set GRAM area
    ili9320WriteReg(0x0020, 0x0000);    // GRAM horisontal address
    ili9320WriteReg(0x0021, 0x0000);    // GRAM vertical address

    ili9320WriteReg(0x0050, 0x0000);    // Horizontal GRAM Start Address
    ili9320WriteReg(0x0051, 0x00EF);    // Horizontal GRAM End Address
    ili9320WriteReg(0x0052, 0x0000);    // Vertical GRAM Start Address
    ili9320WriteReg(0x0053, 0x013F);    // Vertical GRAM End Address

    ili9320WriteReg(0x0060, 0x2700);    // Gate scan line
    ili9320WriteReg(0x0061, 0x0003);    // NDV, VLE, REV
    ili9320WriteReg(0x006A, 0x0000);    // Set scrolling line

    // Partial Display Control
    ili9320WriteReg(0x0080, 0x0000);
    ili9320WriteReg(0x0081, 0x0000);
    ili9320WriteReg(0x0082, 0x0000);
    ili9320WriteReg(0x0083, 0x0000);
    ili9320WriteReg(0x0084, 0x0000);
    ili9320WriteReg(0x0085, 0x0000);

    // Panel Control
    ili9320WriteReg(0x0090, 0x0010);
    ili9320WriteReg(0x0092, 0x0000);
    ili9320WriteReg(0x0093, 0x0003);
    ili9320WriteReg(0x0095, 0x0110);
    ili9320WriteReg(0x0097, 0x0000);
    ili9320WriteReg(0x0098, 0x0000);

    ili9320WriteReg(0x0007, 0x0173);    // 262K color and display ON

    SET(DISP_CS);
}

void ili9320Init(DispDriver **driver)
{
    *driver = &drv;
    ili9320InitSeq();
}

void ili9320Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        ili9320WriteReg(0x0001, 0x0000);    // Set SS and SM bit
        ili9320WriteReg(0x0060, 0xA700);    // Gate scan line
    } else {
        ili9320WriteReg(0x0001, 0x0100);    // Set SS and SM bit
        ili9320WriteReg(0x0060, 0x2700);    // Gate scan line
    }

    SET(DISP_CS);
}

void ili9320Shift(int16_t value)
{
    CLR(DISP_CS);

    ili9320WriteReg(0x006A, (uint16_t)value);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9320Sleep(void)
{
    CLR(DISP_CS);

    ili9320WriteReg(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    ili9320WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    ili9320WriteReg(0x0012, 0x0000);    // VREG1OUT voltage
    ili9320WriteReg(0x0013, 0x0000);    // VDV[4:0] for VCOM amplitude
    LL_mDelay(200);
    ili9320WriteReg(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9320Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    ili9320WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    ili9320WriteReg(0x0012, 0x0000);    // VREG1OUT voltage
    ili9320WriteReg(0x0013, 0x0000);    // VDV[4:0] for VCOM amplitude
    LL_mDelay(200);
    ili9320WriteReg(0x0010, 0x17B0);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0037);    // DC1[2:0], DC0[2:0], VC[2:0]
    LL_mDelay(50);
    ili9320WriteReg(0x0012, 0x013A);    // VREG1OUT voltage
    LL_mDelay(50);
    ili9320WriteReg(0x0013, 0x1600);    // VDV[4:0] for VCOM amplitude
    ili9320WriteReg(0x0029, 0x000C);    // VCM[4:0] for VCOMH

    ili9320WriteReg(0x0007, 0x0173);    // 262K color and display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9320DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9320SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9320DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9320SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9320DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    ili9320SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

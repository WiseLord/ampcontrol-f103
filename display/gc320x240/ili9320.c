#include "ili9320.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define ILI9320_WIDTH           240
#define ILI9320_HEIGHT          320
#define ILI9320_PIXELS          (ILI9320_WIDTH * ILI9320_HEIGHT)

static DispDriver drv = {
    .width = ILI9320_HEIGHT,
    .height = ILI9320_WIDTH,
    .drawPixel = ili9320DrawPixel,
    .drawRectangle = ili9320DrawRectangle,
    .drawImage = ili9320DrawImage,
    .rotate = ili9320Rotate,
};

static inline void ili9320SelectReg(uint16_t reg) __attribute__((always_inline));
static inline void ili9320SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

static void ili9320WriteReg(uint16_t reg, uint16_t data)
{
    ili9320SelectReg(reg);
    dispdrvSendData16(data);
}

static inline void ili9320InitSeq(void)
{
    // Wait for reset
    LL_mDelay(50);

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



static inline void ili9320SetWindow(uint16_t x, uint16_t y, uint16_t w,
                                    uint16_t h) __attribute__((always_inline));
static inline void ili9320SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9320WriteReg(0x0050, y);
    ili9320WriteReg(0x0051, y + h - 1);
    ili9320WriteReg(0x0052, x);
    ili9320WriteReg(0x0053, x + w - 1);

    // Set cursor
    ili9320WriteReg(0x0020, y);
    ili9320WriteReg(0x0021, x);

    // Select RAM mode
    ili9320SelectReg(0x0022);
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

    SET(DISP_CS);
}

void ili9320DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9320SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void ili9320DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9320SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void ili9320DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_CS);

    ili9320SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

#include "ssd2119.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = ssd2119DrawPixel,
    .drawRectangle = ssd2119DrawRectangle,
    .drawImage = ssd2119DrawImage,
    .rotate = ssd2119Rotate,
    .shift = ssd2119Shift,
};

__attribute__((always_inline))
static inline void ssd2119SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void ssd2119WriteReg(uint16_t reg, uint16_t value)
{
    ssd2119SelectReg(reg);
    dispdrvSendData16(value);
}

static inline void ssd2119SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    ssd2119WriteReg(0x0044, (uint16_t)(y | (y1 << 8)));
    ssd2119WriteReg(0x0045, (uint16_t)x);
    ssd2119WriteReg(0x0046, (uint16_t)x1);

    // Set cursor
    ssd2119WriteReg(0x004E, (uint16_t)x);
    ssd2119WriteReg(0x004F, (uint16_t)y);

    // Select RAM mode
    ssd2119SelectReg(0x0022);
}

static inline void ssd2119InitSeq(void)
{
    CLR(DISP_CS);

    ssd2119WriteReg(0x0028, 0x0006);    // VCOM OTP
    ssd2119WriteReg(0x0000, 0x0001);    // Start internal OSC
    ssd2119WriteReg(0x0001, 0x72EF);    // Driver Output Control
    ssd2119WriteReg(0x0002, 0x0400);
    ssd2119WriteReg(0x0003, 0x6064);    // Sleep=0
    ssd2119WriteReg(0x0010, 0x0000);

    ssd2119WriteReg(0x0011, 0x6238);
    ssd2119WriteReg(0x0007, 0x0000);
    ssd2119WriteReg(0x0025, 0xD000);
    ssd2119WriteReg(0x000B, 0x53c8);

    // Adjust the Gamma Curve
    ssd2119WriteReg(0x0030, 0x0000);
    ssd2119WriteReg(0x0031, 0x0101);
    ssd2119WriteReg(0x0032, 0x0100);
    ssd2119WriteReg(0x0033, 0x0305);
    ssd2119WriteReg(0x0034, 0x0707);
    ssd2119WriteReg(0x0035, 0x0305);
    ssd2119WriteReg(0x0036, 0x0707);
    ssd2119WriteReg(0x0037, 0x0201);
    ssd2119WriteReg(0x003A, 0x1200);
    ssd2119WriteReg(0x003B, 0x0900);

    // Power On Sequence
    ssd2119WriteReg(0x000C, 0x0004);
    LL_mDelay(50);
    ssd2119WriteReg(0x000D, 0x0009);
    LL_mDelay(50);
    ssd2119WriteReg(0x001E, 0x0068);
    LL_mDelay(50);
    ssd2119WriteReg(0x000E, 0x2700);
    LL_mDelay(50);
    ssd2119WriteReg(0x0026, 0x7C00);
    ssd2119WriteReg(0x0027, 0x006D);

    // Set GRAM area
    ssd2119WriteReg(0x004E, 0x0000);    // GRAM horisontal address
    ssd2119WriteReg(0x004F, 0x0000);    // GRAM vertical address

    ssd2119WriteReg(0x0044, 0xEF00);    // Vertical GRAM End Start Address
    ssd2119WriteReg(0x0045, 0x0000);    // Horizontal GRAM Start Address
    ssd2119WriteReg(0x0046, 0x013F);    // Horizontal GRAM End Address

    ssd2119WriteReg(0x0007, 0x0033);    // 65K color and display ON*/

    SET(DISP_CS);
}

void ssd2119Init(DispDriver **driver)
{
    *driver = &drv;
    ssd2119InitSeq();
}

void ssd2119Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        ssd2119WriteReg(0x0001, 0x0000);    // Set SS and SM bit
        ssd2119WriteReg(0x0060, 0xA700);    // Gate scan line
    } else {
        ssd2119WriteReg(0x0001, 0x0100);    // Set SS and SM bit
        ssd2119WriteReg(0x0060, 0x2700);    // Gate scan line
    }

    SET(DISP_CS);
}

void ssd2119Shift(int16_t value)
{
    CLR(DISP_CS);

    ssd2119WriteReg(0x006A, (uint16_t)value);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119Sleep(void)
{
    CLR(DISP_CS);

    ssd2119WriteReg(0x0007, 0x0000);    // Display OFF
    ssd2119WriteReg(0x0010, 0x0002);    // Sleep=0

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    ssd2119WriteReg(0x0010, 0x0000);    // Sleep=0
    ssd2119WriteReg(0x0007, 0x0173);    // display ON

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ssd2119SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ssd2119SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd2119DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    ssd2119SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

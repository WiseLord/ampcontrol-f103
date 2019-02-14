#include "ili9486.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../dispdrv.h"

#define ILI9486_WIDTH           320
#define ILI9486_HEIGHT          480
#define ILI9486_PIXELS          (ILI9486_WIDTH * ILI9486_HEIGHT)

static DispDriver drv = {
    .width = ILI9486_HEIGHT,
    .height = ILI9486_WIDTH,
    .drawPixel = ili9486DrawPixel,
    .drawRectangle = ili9486DrawRectangle,
    .drawImage = ili9486DrawImage,
    .rotate = ili9486Rotate,
};

__attribute__((always_inline))
static inline void ili9486SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void ili9486SelectReg(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void ili9486SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    ili9486SelectReg(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);


    ili9486SelectReg(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    ili9486SelectReg(0x2C);
}

static inline void ili9486InitSeq(void)
{
    // Wait for reset
    LL_mDelay(100);

    CLR(DISP_CS);

    // Initial Sequence
    ili9486SelectReg(0x28);     // Display OFF
    LL_mDelay(100);
    ili9486SelectReg(0x10);     // Sleep IN
    LL_mDelay(50);
    ili9486SelectReg(0xF2);     // ?????
    dispdrvSendData8(0x18);
    dispdrvSendData8(0xA3);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0xB2);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0xFF);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x00);

    ili9486SelectReg(0xF1);     // ?????
    dispdrvSendData8(0x36);
    dispdrvSendData8(0xA4);

    ili9486SelectReg(0xF8);     // ?????
    dispdrvSendData8(0x21);
    dispdrvSendData8(0x04);

    ili9486SelectReg(0xF9);     // ?????
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x08);

    ili9486SelectReg(0xC0);     // Power Control 1
    dispdrvSendData8(0x0d);
    dispdrvSendData8(0x0d);

    ili9486SelectReg(0xC1);     // Power Control 2
    dispdrvSendData8(0x41);
    dispdrvSendData8(0x00);

    ili9486SelectReg(0xC2);     // Power Control 3
    dispdrvSendData8(0x00);

    ili9486SelectReg(0xC3);     // Power Control 4
    dispdrvSendData8(0x33);

    ili9486SelectReg(0xC4);     // Power Control 5
    dispdrvSendData8(0x33);

    ili9486SelectReg(0xC5);     // VCOM Control
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x53);

    ili9486SelectReg(0xB6);     // Display Function Control
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x42);     // 0x22 = Rotate display 180 deg.
    dispdrvSendData8(0x3B);

    ili9486SelectReg(0xE0);     // PGAMCTRL (Positive Gamma Control)
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x1F);
    dispdrvSendData8(0x1C);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x48);
    dispdrvSendData8(0x98);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0x13);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x00);

    ili9486SelectReg(0xE1);     // NGAMCTRL (Negative Gamma Control)
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x32);
    dispdrvSendData8(0x2E);
    dispdrvSendData8(0x0B);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x05);
    dispdrvSendData8(0x47);
    dispdrvSendData8(0x75);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x20);
    dispdrvSendData8(0x00);

    ili9486SelectReg(0x20);     // Display Inversion OFF
    dispdrvSendData8(0x00);

    ili9486SelectReg(0x36);     // Memory Access Control
    dispdrvSendData8(0x48);

    ili9486SelectReg(0x3A);     // Interface Pixel Format
    dispdrvSendData8(0x55);

    ili9486SelectReg(0xB4);     // Display Inversion Control
    dispdrvSendData8(0x00);

    ili9486SelectReg(0x2A);     // Column Addess Set
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0xDF);

    ili9486SelectReg(0x2B);     // Page Address Set
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x3f);

    ili9486SelectReg(0x11);     // Sleep OUT

    LL_mDelay(120);
    ili9486SelectReg(0x29);     // Display ON
    ili9486SelectReg(0x13);     // Normal Display Mode ON
    ili9486SelectReg(0x2C);     // Memory Write

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9486Init(DispDriver **driver)
{
    *driver = &drv;
    ili9486InitSeq();
}

void ili9486Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        ili9486SelectReg(0x36);
        dispdrvSendData8(0x42);
    } else {
        ili9486SelectReg(0x36);
        dispdrvSendData8(0x22);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9486Sleep(void)
{
    CLR(DISP_CS);

    ili9486SelectReg(0x28);    // Display OFF
    LL_mDelay(100);
    ili9486SelectReg(0x10);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9486Wakeup(void)
{
    CLR(DISP_CS);

    ili9486SelectReg(0x11);    // Display ON
    LL_mDelay(100);
    ili9486SelectReg(0x29);

    SET(DISP_CS);
}

void ili9486DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9486SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void ili9486DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9486SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void ili9486DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    ili9486SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

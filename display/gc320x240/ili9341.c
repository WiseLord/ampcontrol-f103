#include "ili9341.h"
#include "ili9341_regs.h"

#include <stm32f1xx_ll_utils.h>
#include <stm32f1xx_ll_spi.h>
#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = ili9341DrawPixel,
    .drawRectangle = ili9341DrawRectangle,
    .drawImage = ili9341DrawImage,
    .rotate = ili9341Rotate,
    .shift = ili9341Shift,
};

__attribute__((always_inline))
static inline void ili9341SelectReg(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void ili9341SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    ili9341SelectReg(ILI9341_CASET);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    ili9341SelectReg(ILI9341_PASET);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    ili9341SelectReg(ILI9341_RAMWR);
}

static void ili9341InitSeq(void)
{
    CLR(DISP_CS);

    ili9341SelectReg(ILI9341_SWRESET);
    LL_mDelay(10);
    ili9341SelectReg(ILI9341_DISPOFF);

    ili9341SelectReg(ILI9341_PWCTRLB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x30);

    ili9341SelectReg(ILI9341_PWSEQCTL);
    dispdrvSendData8(0x64);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x81);

    ili9341SelectReg(ILI9341_DRVTIMCTLA1);
    dispdrvSendData8(0x85);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x79);

    ili9341SelectReg(ILI9341_PWCTRLA);
    dispdrvSendData8(0x39);
    dispdrvSendData8(0x2C);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x02);

    ili9341SelectReg(ILI9341_PUMPRTCTL);
    dispdrvSendData8(0x20);

    ili9341SelectReg(ILI9341_DRVTIMB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ili9341SelectReg(ILI9341_PWCTRL1);
    dispdrvSendData8(0x26);

    ili9341SelectReg(ILI9341_PWCTRL2);
    dispdrvSendData8(0x11);

    ili9341SelectReg(ILI9341_VMCTRL1);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x3E);

    ili9341SelectReg(ILI9341_VMCTRL2);
    dispdrvSendData8(0xBE);

    ili9341SelectReg(ILI9341_PIXSET);
    dispdrvSendData8(0x55);

    ili9341SelectReg(ILI9341_FRMCTR1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x1B);

    ili9341SelectReg(ILI9341_EN3G);
    dispdrvSendData8(0x08);

    ili9341SelectReg(ILI9341_GAMSET);
    dispdrvSendData8(0x01);

    ili9341SelectReg(ILI9341_ETMOD);
    dispdrvSendData8(0x07);

    ili9341SelectReg(ILI9341_DISCTRL);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0xE2);
    dispdrvSendData8(0x27);

    // Retate
    ili9341SelectReg(ILI9341_MADCTL);
    dispdrvSendData8(0x08);

    ili9341SelectReg(ILI9341_PGAMCTRL);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x2B);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x4E);
    dispdrvSendData8(0xF1);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x09);
    dispdrvSendData8(0x00);

    ili9341SelectReg(ILI9341_NGAMCTRL);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x48);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x36);
    dispdrvSendData8(0x0F);

    ili9341SelectReg(ILI9341_SLPOUT);
    ili9341SelectReg(ILI9341_DISPON);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Init(DispDriver **driver)
{
    *driver = &drv;
    ili9341InitSeq();
}

void ili9341Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        ili9341SelectReg(ILI9341_DISCTRL);
        dispdrvSendData8(0x0A);
        dispdrvSendData8(0x82);
    } else {
        ili9341SelectReg(ILI9341_DISCTRL);
        dispdrvSendData8(0x0A);
        dispdrvSendData8(0xE2);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Shift(int16_t value)
{
    CLR(DISP_CS);

    ili9341SelectReg(ILI9341_VSCRDEF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);
    dispdrvSendData8((drv.width >> 8) & 0xFF);
    dispdrvSendData8(drv.width & 0xFF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);

    ili9341SelectReg(ILI9341_VSCRSADD);
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Sleep(void)
{
    CLR(DISP_CS);

    ili9341SelectReg(ILI9341_SLPIN);

    SET(DISP_CS);
}

void ili9341Wakeup(void)
{
    CLR(DISP_CS);

    ili9341SelectReg(ILI9341_SLPOUT);

    SET(DISP_CS);
}

void ili9341DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9341SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9341SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    ili9341SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

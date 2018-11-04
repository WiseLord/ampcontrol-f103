#include "ili9341.h"
#include "ili9341_regs.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define ILI9341_WIDTH           240
#define ILI9341_HEIGHT          320
#define ILI9341_PIXELS          (ILI9341_WIDTH * ILI9341_HEIGHT)

static DispDriver drv = {
    .width = ILI9341_HEIGHT,
    .height = ILI9341_WIDTH,
    .drawPixel = ili9341DrawPixel,
    .drawRectangle = ili9341DrawRectangle,
    .drawImage = ili9341DrawImage,
};

static inline void ili9341SendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void ili9341SendCmd(uint8_t cmd)
{
    dispdrvWaitOperation();
    CLR(DISP_RS);

    dispdrvSendData8(cmd);

    dispdrvWaitOperation();
    SET(DISP_RS);
}

static void ili9341InitSeq(void)
{
    LL_mDelay(50);

    CLR(DISP_CS);

    ili9341SendCmd(ILI9341_SWRESET);
    LL_mDelay(10);
    ili9341SendCmd(ILI9341_DISPOFF);

    ili9341SendCmd(ILI9341_PWCTRLB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x30);

    ili9341SendCmd(ILI9341_PWSEQCTL);
    dispdrvSendData8(0x64);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x81);

    ili9341SendCmd(ILI9341_DRVTIMCTLA1);
    dispdrvSendData8(0x85);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x79);

    ili9341SendCmd(ILI9341_PWCTRLA);
    dispdrvSendData8(0x39);
    dispdrvSendData8(0x2C);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x02);

    ili9341SendCmd(ILI9341_PUMPRTCTL);
    dispdrvSendData8(0x20);

    ili9341SendCmd(ILI9341_DRVTIMB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ili9341SendCmd(ILI9341_PWCTRL1);
    dispdrvSendData8(0x26);

    ili9341SendCmd(ILI9341_PWCTRL2);
    dispdrvSendData8(0x11);

    ili9341SendCmd(ILI9341_VMCTRL1);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x3E);

    ili9341SendCmd(ILI9341_VMCTRL2);
    dispdrvSendData8(0xBE);

    ili9341SendCmd(ILI9341_PIXSET);
    dispdrvSendData8(0x55);

    ili9341SendCmd(ILI9341_FRMCTR1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x1B);

    ili9341SendCmd(ILI9341_EN3G);
    dispdrvSendData8(0x08);

    ili9341SendCmd(ILI9341_GAMSET);
    dispdrvSendData8(0x01);

    ili9341SendCmd(ILI9341_ETMOD);
    dispdrvSendData8(0x07);

    ili9341SendCmd(ILI9341_DISCTRL);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x82);
    dispdrvSendData8(0x27);

    // Retate
    ili9341SendCmd(ILI9341_MADCTL);
    dispdrvSendData8(0x08);

    ili9341SendCmd(ILI9341_PGAMCTRL);
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

    ili9341SendCmd(ILI9341_NGAMCTRL);
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

    ili9341SendCmd(ILI9341_SLPOUT);
    ili9341SendCmd(ILI9341_DISPON);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ili9341SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9341SendCmd(ILI9341_PASET);
    dispdrvSendData16(x);
    dispdrvSendData16(x + w - 1);

    ili9341SendCmd(ILI9341_CASET);
    dispdrvSendData16(y);
    dispdrvSendData16(y + h - 1);

    ili9341SendCmd(ILI9341_RAMWR);
}

void ili9341Init(DispDriver **driver)
{
    *driver = &drv;

    CLR(DISP_RST);
    LL_mDelay(100);
    SET(DISP_RST);

    // Init magic
    ili9341InitSeq();
    SET(DISP_BCKL);
}

void ili9341Sleep(void)
{
    CLR(DISP_CS);

    ili9341SendCmd(ILI9341_SLPIN);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ili9341Wakeup(void)
{
    CLR(DISP_CS);

    ili9341SendCmd(ILI9341_SLPOUT);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ili9341DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9341SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ili9341DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9341SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

void ili9341DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_CS);

    ili9341SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    dispdrvWaitOperation();
    SET(DISP_CS);
}

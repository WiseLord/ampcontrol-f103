#include "ili9341.h"
#include "ili9341_regs.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = ili9341Init,
    .setWindow = ili9341SetWindow,
    .rotate = ili9341Rotate,
    .shift = ili9341Shift,
};

void ili9341Init(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(ILI9341_SWRESET);
    LL_mDelay(10);
    dispdrvSelectReg8(ILI9341_DISPOFF);

    dispdrvSelectReg8(ILI9341_PWCTRLB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x30);

    dispdrvSelectReg8(ILI9341_PWSEQCTL);
    dispdrvSendData8(0x64);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x81);

    dispdrvSelectReg8(ILI9341_DRVTIMCTLA1);
    dispdrvSendData8(0x85);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x79);

    dispdrvSelectReg8(ILI9341_PWCTRLA);
    dispdrvSendData8(0x39);
    dispdrvSendData8(0x2C);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x02);

    dispdrvSelectReg8(ILI9341_PUMPRTCTL);
    dispdrvSendData8(0x20);

    dispdrvSelectReg8(ILI9341_DRVTIMB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(ILI9341_PWCTRL1);
    dispdrvSendData8(0x26);

    dispdrvSelectReg8(ILI9341_PWCTRL2);
    dispdrvSendData8(0x11);

    dispdrvSelectReg8(ILI9341_VMCTRL1);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x3E);

    dispdrvSelectReg8(ILI9341_VMCTRL2);
    dispdrvSendData8(0xBE);

    dispdrvSelectReg8(ILI9341_PIXSET);
    dispdrvSendData8(0x55);

    dispdrvSelectReg8(ILI9341_FRMCTR1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x1B);

    dispdrvSelectReg8(ILI9341_EN3G);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(ILI9341_GAMSET);
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(ILI9341_ETMOD);
    dispdrvSendData8(0x07);

    dispdrvSelectReg8(ILI9341_DISCTRL);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0xE2);
    dispdrvSendData8(0x27);

    // Retate
    dispdrvSelectReg8(ILI9341_MADCTL);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(ILI9341_PGAMCTRL);
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

    dispdrvSelectReg8(ILI9341_NGAMCTRL);
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

    dispdrvSelectReg8(ILI9341_SLPOUT);
    dispdrvSelectReg8(ILI9341_DISPON);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvSelectReg8(ILI9341_DISCTRL);
        dispdrvSendData8(0x0A);
        dispdrvSendData8(0x82);
    } else {
        dispdrvSelectReg8(ILI9341_DISCTRL);
        dispdrvSendData8(0x0A);
        dispdrvSendData8(0xE2);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(ILI9341_VSCRDEF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);
    dispdrvSendData8((dispdrv.width >> 8) & 0xFF);
    dispdrvSendData8(dispdrv.width & 0xFF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);

    dispdrvSelectReg8(ILI9341_VSCRSADD);
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9341Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(ILI9341_SLPIN);

    SET(DISP_CS);
}

void ili9341Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(ILI9341_SLPOUT);

    SET(DISP_CS);
}

void ili9341SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvSelectReg8(ILI9341_CASET);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    dispdrvSelectReg8(ILI9341_PASET);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    dispdrvSelectReg8(ILI9341_RAMWR);
}

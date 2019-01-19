#include "r61581.h"

#include <stm32f1xx_ll_utils.h>
#include <stm32f1xx_ll_spi.h>
#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 480,
    .height = 320,
    .drawPixel = r61581DrawPixel,
    .drawRectangle = r61581DrawRectangle,
    .drawImage = r61581DrawImage,
    .rotate = r61581Rotate,
    .shift = r615811Shift,
};

__attribute__((always_inline))
static inline void r61581SelectReg(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void r61581SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    r61581SelectReg(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    r61581SelectReg(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    r61581SelectReg(0x2C);
}

__attribute__((always_inline))
static inline void r61581InitSeq(void)
{
    CLR(DISP_CS);

    // Initial Sequence

    r61581SelectReg(0xB0);  // Manufacturer command access protect
    dispdrvSendData8(0x00); // Enable all

    r61581SelectReg(0x28);  // Set display off
    LL_mDelay(30);

    r61581SelectReg(0xB3);  // Frame memory access and interface setting
    dispdrvSendData8(0x02); // WEMODE=1
    dispdrvSendData8(0x00); // TE=every frame
    dispdrvSendData8(0x00); // DENC=every frame
    dispdrvSendData8(0x10); // 16bit format

    r61581SelectReg(0xB4);  // Display mode and frame memory write mode setting
    dispdrvSendData8(0x00); // DBI and Internal oscillator clock

    r61581SelectReg(0xC0);  // Panel driving setting
    dispdrvSendData8(0x16); // REV, BGR, SS
    dispdrvSendData8(0x3B); // NL=0x3B => 480 lines
    dispdrvSendData8(0x00); // SCN=0x00 => Scanning start position
    dispdrvSendData8(0x00); // Line inversion during retrace period and non-lit display area
    dispdrvSendData8(0x00); // Some setting for non-lit display area
    dispdrvSendData8(0x01); // 3 frames interval in non-lit display area
    dispdrvSendData8(0x00); // Some setting for inversion
    dispdrvSendData8(0x43); // 4 clocks on PCLKD=High, 3 clocks on PCLKD=Low

    r61581SelectReg(0xC1);  // Display timing setting for normal mode
    dispdrvSendData8(0x08); // BC0=1 => Line inversion waveform is selected
    dispdrvSendData8(0x15); // RTN0=0x15 => 15 clocks in 1 line period
    dispdrvSendData8(0x08); // BP0=0x08 => 8 back porch lines
    dispdrvSendData8(0x08); // FP0=0x08 => 8 front porch lines

    r61581SelectReg(0xC4);  // Source/VCOM/Gate driving timing setting
    dispdrvSendData8(0x15); // SDT=1, NOW=5
    dispdrvSendData8(0x03); // MCP=3
    dispdrvSendData8(0x03); // VEM=3
    dispdrvSendData8(0x01); // SPCW=3

    r61581SelectReg(0xC6);  // Interface setting
    dispdrvSendData8(0x02); // EPL=1

    r61581SelectReg(0xC8);  // Gamma set
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x05);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0x6B);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x15);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x60);

    r61581SelectReg(0x36);  // Set address mode
    dispdrvSendData8(0x40); // B7=0 => Top to bottom, B6=1 => Right to left, B5=0 => Normal mode, B4=0 => Refresh top to bottom, B0=0 => Normal

    r61581SelectReg(0x3A);  // Set pixel format
    dispdrvSendData8(0x55); // DPI=5 => 16bits/pixel, DBI=5 => 16bits/pixel

    r61581SelectReg(0x38);  // Exit idle mode

    r61581SelectReg(0xD0);  // Power setting
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0xA2);

    r61581SelectReg(0xD1);  // VCOM setting
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x5A);
    dispdrvSendData8(0x10);

    r61581SelectReg(0xD2);  // Power setting for normal mode
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x04);

    r61581SelectReg(0x11);  // Exit sleep mode
    LL_mDelay(150);

    r61581SelectReg(0x29);  // Set display on
    LL_mDelay(30);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void r61581Init(DispDriver **driver)
{
    *driver = &drv;
    r61581InitSeq();
}

void r61581Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        r61581SelectReg(0xC0);  // Panel driving setting
        dispdrvSendData8(0x13); // REV, BGR, SS
    } else {
        r61581SelectReg(0xC0);  // Panel driving setting
        dispdrvSendData8(0x16); // REV, BGR, SS
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void r615811Shift(int16_t value)
{
    CLR(DISP_CS);

    r61581SelectReg(0x33);
    dispdrvSendData8(0);
    dispdrvSendData8(0);
    dispdrvSendData8((drv.width >> 8) & 0xFF);
    dispdrvSendData8(drv.width & 0xFF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);

    r61581SelectReg(0x37);
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void r61581Sleep(void)
{
    CLR(DISP_CS);

    r61581SelectReg(0x28);    // Display OFF
    LL_mDelay(100);
    r61581SelectReg(0x10);

    SET(DISP_CS);
}

void r61581Wakeup(void)
{
    CLR(DISP_CS);

    r61581SelectReg(0x11);    // Display ON
    LL_mDelay(100);
    r61581SelectReg(0x29);

    SET(DISP_CS);
}

void r61581DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    r61581SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void r61581DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    r61581SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void r61581DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    r61581SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

#include "../dispdrv.h"

void r61581Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence

    dispdrvSelectReg8(0xB0);  // Manufacturer command access protect
    dispdrvSendData8(0x00); // Enable all

    dispdrvSelectReg8(0x28);  // Set display off
    DISP_MDELAY(30);

    dispdrvSelectReg8(0xB3);  // Frame memory access and interface setting
    dispdrvSendData8(0x02); // WEMODE=1
    dispdrvSendData8(0x00); // TE=every frame
    dispdrvSendData8(0x00); // DENC=every frame
    dispdrvSendData8(0x10); // 16bit format

    dispdrvSelectReg8(0xB4);  // Display mode and frame memory write mode setting
    dispdrvSendData8(0x00); // DBI and Internal oscillator clock

    dispdrvSelectReg8(0xC0);  // Panel driving setting
    dispdrvSendData8(0x16); // REV, BGR, SS
    dispdrvSendData8(0x3B); // NL=0x3B => 480 lines
    dispdrvSendData8(0x00); // SCN=0x00 => Scanning start position
    dispdrvSendData8(0x00); // Line inversion during retrace period and non-lit display area
    dispdrvSendData8(0x00); // Some setting for non-lit display area
    dispdrvSendData8(0x01); // 3 frames interval in non-lit display area
    dispdrvSendData8(0x00); // Some setting for inversion
    dispdrvSendData8(0x43); // 4 clocks on PCLKD=High, 3 clocks on PCLKD=Low

    dispdrvSelectReg8(0xC1);  // Display timing setting for normal mode
    dispdrvSendData8(0x08); // BC0=1 => Line inversion waveform is selected
    dispdrvSendData8(0x15); // RTN0=0x15 => 15 clocks in 1 line period
    dispdrvSendData8(0x08); // BP0=0x08 => 8 back porch lines
    dispdrvSendData8(0x08); // FP0=0x08 => 8 front porch lines

    dispdrvSelectReg8(0xC4);  // Source/VCOM/Gate driving timing setting
    dispdrvSendData8(0x15); // SDT=1, NOW=5
    dispdrvSendData8(0x03); // MCP=3
    dispdrvSendData8(0x03); // VEM=3
    dispdrvSendData8(0x01); // SPCW=3

    dispdrvSelectReg8(0xC6);  // Interface setting
    dispdrvSendData8(0x02); // EPL=1

    dispdrvSelectReg8(0xC8);  // Gamma set
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

    dispdrvSelectReg8(0x36);  // Set address mode
    dispdrvSendData8(
        0x40); // B7=0 => Top to bottom, B6=1 => Right to left, B5=0 => Normal mode, B4=0 => Refresh top to bottom, B0=0 => Normal

    dispdrvSelectReg8(0x3A);  // Set pixel format
    dispdrvSendData8(0x55); // DPI=5 => 16bits/pixel, DBI=5 => 16bits/pixel

    dispdrvSelectReg8(0x38);  // Exit idle mode

    dispdrvSelectReg8(0xD0);  // Power setting
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0xA2);

    dispdrvSelectReg8(0xD1);  // VCOM setting
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x5A);
    dispdrvSendData8(0x10);

    dispdrvSelectReg8(0xD2);  // Power setting for normal mode
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x04);

    dispdrvSelectReg8(0x11);  // Exit sleep mode
    DISP_MDELAY(150);

    dispdrvSelectReg8(0x29);  // Set display on
    DISP_MDELAY(30);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void r61581Rotate(bool rotate)
{
    CLR(DISP_CS);

    if (rotate) {
        dispdrvSelectReg8(0xC0);  // Panel driving setting
        dispdrvSendData8(0x13); // REV, BGR, SS
    } else {
        dispdrvSelectReg8(0xC0);  // Panel driving setting
        dispdrvSendData8(0x16); // REV, BGR, SS
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void r615811Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x33);
    dispdrvSendData8(0);
    dispdrvSendData8(0);
    dispdrvSendData8((dispdrv.width >> 8) & 0xFF);
    dispdrvSendData8(dispdrv.width & 0xFF);
    dispdrvSendData8(0);
    dispdrvSendData8(0);

    dispdrvSelectReg8(0x37);
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void r61581Sleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
        dispdrvSelectReg8(0x28);    // Display OFF
        DISP_MDELAY(100);
        dispdrvSelectReg8(0x10);
    } else {
        dispdrvSelectReg8(0x11);    // Display ON
        DISP_MDELAY(100);
        dispdrvSelectReg8(0x29);
    }

    SET(DISP_CS);
}

void r61581SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvSelectReg8(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2C);
}

const DispDriver dispdrv = {
    .width = 480,
    .height = 320,
    .init = r61581Init,
    .sleep = r61581Sleep,
    .setWindow = r61581SetWindow,
    .rotate = r61581Rotate,
    .shift = r615811Shift,
};

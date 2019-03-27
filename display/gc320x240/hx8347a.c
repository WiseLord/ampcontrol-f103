#include "hx8347a.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = hx8347aDrawPixel,
    .drawRectangle = hx8347aDrawRectangle,
    .drawImage = hx8347aDrawImage,
};

__attribute__((always_inline))
static inline void hx8347aSelectReg(uint8_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void hx8347aWriteReg(uint8_t reg, uint8_t value)
{
    hx8347aSelectReg(reg);
    dispdrvSendData8(value);
}

__attribute__((always_inline))
static inline void hx8347aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    hx8347aWriteReg(0x03, ((y >> 0) & 0xFF));
    hx8347aWriteReg(0x05, ((y1 >> 0) & 0xFF));

    hx8347aWriteReg(0x06, ((x >> 8) & 0xFF));
    hx8347aWriteReg(0x07, ((x >> 0) & 0xFF));
    hx8347aWriteReg(0x08, ((x1 >> 8) & 0xFF));
    hx8347aWriteReg(0x09, ((x1 >> 0) & 0xFF));

    hx8347aSelectReg(0x22);
}

static inline void hx8347aInitSeq(void)
{
    CLR(DISP_CS);

    // Gamma for CMO 2.8
    hx8347aWriteReg(0x46, 0x95);
    hx8347aWriteReg(0x47, 0x51);
    hx8347aWriteReg(0x48, 0x00);
    hx8347aWriteReg(0x49, 0x36);
    hx8347aWriteReg(0x4A, 0x11);
    hx8347aWriteReg(0x4B, 0x66);
    hx8347aWriteReg(0x4C, 0x14);
    hx8347aWriteReg(0x4D, 0x77);
    hx8347aWriteReg(0x4E, 0x13);
    hx8347aWriteReg(0x4F, 0x4C);
    hx8347aWriteReg(0x50, 0x46);
    hx8347aWriteReg(0x51, 0x46);

    //240x320 window setting
    hx8347aWriteReg(0x02, 0x00); // Column address start2
    hx8347aWriteReg(0x03, 0x00); // Column address start1
    hx8347aWriteReg(0x04, 0x00); // Column address end2
    hx8347aWriteReg(0x05, 0xEF); // Column address end1
    hx8347aWriteReg(0x06, 0x00); // Row address start2
    hx8347aWriteReg(0x07, 0x00); // Row address start1
    hx8347aWriteReg(0x08, 0x01); // Row address end2
    hx8347aWriteReg(0x09, 0x3F); // Row address end1

    // Display Setting
    hx8347aWriteReg(0x01, 0x06); // IDMON=0, INVON=1, NORON=1, PTLON=0
    hx8347aWriteReg(0x16, 0x48); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0

    hx8347aWriteReg(0x23, 0x95); // N_DC=1001 0101
    hx8347aWriteReg(0x24, 0x95); // P_DC=1001 0101
    hx8347aWriteReg(0x25, 0xFF); // I_DC=1111 1111

    hx8347aWriteReg(0x27, 0x06); // N_BP=0000 0110
    hx8347aWriteReg(0x28, 0x06); // N_FP=0000 0110
    hx8347aWriteReg(0x29, 0x06); // P_BP=0000 0110
    hx8347aWriteReg(0x2A, 0x06); // P_FP=0000 0110
    hx8347aWriteReg(0x2C, 0x06); // I_BP=0000 0110
    hx8347aWriteReg(0x2D, 0x06); // I_FP=0000 0110

    hx8347aWriteReg(0x3A, 0x01); // N_RTN=0000, N_NW=001
    hx8347aWriteReg(0x3B, 0x00); // P_RTN=0000, P_NW=000
    hx8347aWriteReg(0x3C, 0xF0); // I_RTN=1111, I_NW=000
    hx8347aWriteReg(0x3D, 0x00); // DIV=00
    LL_mDelay(20);
    hx8347aWriteReg(0x35, 0x38); // EQS=38h
    hx8347aWriteReg(0x36, 0x78); // EQP=78h

    hx8347aWriteReg(0x3E, 0x38); // SON=38h

    hx8347aWriteReg(0x40, 0x0F); // GDON=0Fh
    hx8347aWriteReg(0x41, 0xF0); // GDOFF

    // Power Supply Setting
    hx8347aWriteReg(0x19, 0x49); // OSCADJ=10 0000, OSD_EN=1 //60Hz
    hx8347aWriteReg(0x93, 0x0C); // RADJ=1100
    LL_mDelay(10);
    hx8347aWriteReg(0x20, 0x40); // BT=0100

    hx8347aWriteReg(0x1D, 0x07); // VC1=111
    hx8347aWriteReg(0x1E, 0x00); // VC3=000
    hx8347aWriteReg(0x1F, 0x04); // VRH=0100

    // VCOM Setting for CMO 2.8” Panel
    hx8347aWriteReg(0x44, 0x4D); // VCM=101 0000
    hx8347aWriteReg(0x45, 0x11); // VDV=1 0001
    LL_mDelay(10);

    hx8347aWriteReg(0x1C, 0x04); // AP=100
    LL_mDelay(20);
    hx8347aWriteReg(0x1B, 0x18); // GASENB=0, PON=1, DK=1, XDK=0, DDVDH_TRI=0, STB=0
    LL_mDelay(40);
    hx8347aWriteReg(0x1B, 0x10); // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    LL_mDelay(40);

    hx8347aWriteReg(0x43, 0x80); //Set VCOMG=1
    LL_mDelay(100);

    // Display ON Setting
    hx8347aWriteReg(0x90, 0x7F); // SAP=0111 1111
    hx8347aWriteReg(0x26, 0x04); //GON=0, DTE=0, D=01
    LL_mDelay(40);
    hx8347aWriteReg(0x26, 0x24); //GON=1, DTE=0, D=01
    hx8347aWriteReg(0x26, 0x2C); //GON=1, DTE=0, D=11
    LL_mDelay(40);

    hx8347aWriteReg(0x26, 0x3C); //GON=1, DTE=1, D=11

    // Internal register setting
    hx8347aWriteReg(0x57, 0x02); //Test_Mode Enable
    hx8347aWriteReg(0x95, 0x01); // Set Display clock and Pumping clock to synchronize
    hx8347aWriteReg(0x57, 0x00); // Test_Mode Disable

    SET(DISP_CS);
}

void hx8347aInit(DispDriver **driver)
{
    *driver = &drv;
    hx8347aInitSeq();
}

void hx8347aSleep(void)
{
    CLR(DISP_CS);

    // Display Off
    hx8347aWriteReg(0x26, 0x38); //GON=1, DTE=1, D=10
    LL_mDelay(40);
    hx8347aWriteReg(0x26, 0x28); //GON=1, DTE=0, D=10
    LL_mDelay(40);
    hx8347aWriteReg(0x26, 0x00); //GON=0, DTE=0, D=00
    // Power Off
    hx8347aWriteReg(0x43, 0x00); // VCOMG=0
    LL_mDelay(10);
    hx8347aWriteReg(0x1B, 0x00); // GASENB=0, PON=0, DK=0, XDK=0,
    // VLCD_TRI=0, STB=0
    LL_mDelay(10);
    hx8347aWriteReg(0x1B, 0x08); // GASENB=0, PON=0, DK=1, XDK=0,
    // VLCD_TRI=0, STB=0
    LL_mDelay(10);
    hx8347aWriteReg(0x1C, 0x00); // AP=000
    LL_mDelay(10);
    hx8347aWriteReg(0x90, 0x00); // SAP=00000000
    LL_mDelay(10);
    // Into STB mode
    hx8347aWriteReg(0x1B, 0x09); // GASSENB=0, PON=0, DK=1, XDK=0,
    // VLCD_TRI=0, STB=1
    LL_mDelay(10);
    // Stop Oscillation
    hx8347aWriteReg(0x19, 0x48); // CADJ=0100, CUADJ=100, OSD_EN=0

    SET(DISP_CS);
}

void hx8347aWakeup(void)
{
    CLR(DISP_CS);

    // Start Oscillation
    hx8347aWriteReg(0x0019, 0x0049); // OSCADJ=100 010(FR:60Hz), OSD_EN=1
    LL_mDelay(10);

    // Exit STB mode
    hx8347aWriteReg(0x001B, 0x0008); // NIDSENB=0, PON=0, DK=1, XDK=0,
    // VLCD_TRI=0, STB=0

    // Power Supply Setting
    hx8347aWriteReg(0x20, 0x40); // BT=0100
    hx8347aWriteReg(0x1D, 0x07); // VC1=111
    hx8347aWriteReg(0x1E, 0x00); // VC3=000
    hx8347aWriteReg(0x1F, 0x03); // VRH=0011
    hx8347aWriteReg(0x44, 0x20); // VCM=010 0000
    hx8347aWriteReg(0x45, 0x0E); // VDV=0 1110
    LL_mDelay(10);
    hx8347aWriteReg(0x1C, 0x04); // AP=100
    LL_mDelay(20);
    hx8347aWriteReg(0x1B, 0x18); // NIDSENB=0, PON=1, DK=1, XDK=0,
    LL_mDelay(40);

    // VLCD_TRI=0, STB=0
    hx8347aWriteReg(0x1B, 0x10); // NIDSENB=0, PON=1, DK=0, XDK=0,
    LL_mDelay(40);

    // VLCD_TRI=1, STB=0
    hx8347aWriteReg(0x43, 0x80); // VCOMG=1
    LL_mDelay(100);

    // Display ON Setting
    hx8347aWriteReg(0x90, 0x7F); // SAP=01111111
    LL_mDelay(40);
    hx8347aWriteReg(0x26, 0x04); //GON=0, DTE=0, D=01
    LL_mDelay(40);
    hx8347aWriteReg(0x26, 0x24); //GON=1, DTE=0, D=01
    hx8347aWriteReg(0x26, 0x2C); //GON=1, DTE=0, D=11
    LL_mDelay(40);
    hx8347aWriteReg(0x26, 0x3C); //GON=1, DTE=1, D=11

    SET(DISP_CS);
}

void hx8347aDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    hx8347aSetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void hx8347aDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    hx8347aSetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void hx8347aDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    hx8347aSetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

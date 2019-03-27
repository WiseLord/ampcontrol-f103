#include "hx8347d.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 320,
    .height = 240,
    .drawPixel = hx8347dDrawPixel,
    .drawRectangle = hx8347dDrawRectangle,
    .drawImage = hx8347dDrawImage,
};

__attribute__((always_inline))
static inline void hx8347dSelectReg(uint8_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void hx8347dWriteReg(uint8_t reg, uint8_t value)
{
    hx8347dSelectReg(reg);
    dispdrvSendData8(value);
}

__attribute__((always_inline))
static inline void hx8347dSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    hx8347dWriteReg(0x03, ((y >> 0) & 0xFF));
    hx8347dWriteReg(0x05, ((y1 >> 0) & 0xFF));

    hx8347dWriteReg(0x06, ((x >> 8) & 0xFF));
    hx8347dWriteReg(0x07, ((x >> 0) & 0xFF));
    hx8347dWriteReg(0x08, ((x1 >> 8) & 0xFF));
    hx8347dWriteReg(0x09, ((x1 >> 0) & 0xFF));

    hx8347dSelectReg(0x22);
}

static inline void hx8347dInitSeq(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    //Driving ability Setting

    hx8347dWriteReg(0xEA, 0x00); //PTBA[15:8]
    hx8347dWriteReg(0xEB, 0x20); //PTBA[7:0]
    hx8347dWriteReg(0xEC, 0x0C); //STBA[15:8]
    hx8347dWriteReg(0xED, 0xC4); //STBA[7:0]
    hx8347dWriteReg(0xE8, 0x40); //OPON[7:0]
    hx8347dWriteReg(0xE9, 0x38); //OPON1[7:0]
    hx8347dWriteReg(0xF1, 0x01); //OTPS1B
    hx8347dWriteReg(0xF2, 0x10); //GEN
    hx8347dWriteReg(0x27, 0xA3); //

    //Gamma 2.2 Setting

    hx8347dWriteReg(0x40, 0x00); //
    hx8347dWriteReg(0x41, 0x00); //
    hx8347dWriteReg(0x42, 0x01); //
    hx8347dWriteReg(0x43, 0x12); //
    hx8347dWriteReg(0x44, 0x10); //
    hx8347dWriteReg(0x45, 0x26); //
    hx8347dWriteReg(0x46, 0x08); //
    hx8347dWriteReg(0x47, 0x54); //
    hx8347dWriteReg(0x48, 0x02); //
    hx8347dWriteReg(0x49, 0x15); //
    hx8347dWriteReg(0x4A, 0x19); //
    hx8347dWriteReg(0x4B, 0x19); //
    hx8347dWriteReg(0x4C, 0x16); //

    hx8347dWriteReg(0x50, 0x19); //
    hx8347dWriteReg(0x51, 0x2F); //
    hx8347dWriteReg(0x52, 0x2D); //
    hx8347dWriteReg(0x53, 0x3E); //
    hx8347dWriteReg(0x54, 0x3F); //
    hx8347dWriteReg(0x55, 0x3F); //
    hx8347dWriteReg(0x56, 0x2B); //
    hx8347dWriteReg(0x57, 0x77); //
    hx8347dWriteReg(0x58, 0x09); //
    hx8347dWriteReg(0x59, 0x06); //
    hx8347dWriteReg(0x5A, 0x06); //
    hx8347dWriteReg(0x5B, 0x0A); //
    hx8347dWriteReg(0x5C, 0x1D); //
    hx8347dWriteReg(0x5D, 0xCC); //

    //Power Voltage Setting

    hx8347dWriteReg(0x1B, 0x1B); //VRH=4.65V
    hx8347dWriteReg(0x1A, 0x01); //BT (VGH~15V,VGL~-10V,DDVDH~5V)
    hx8347dWriteReg(0x24, 0x2F); //VMH(VCOM High voltage ~3.2V)
    hx8347dWriteReg(0x25, 0x57); //VML(VCOM Low voltage -1.2V)
    //-------------VCOM offset----------
    hx8347dWriteReg(0x23, 0x79); //for Flicker adjust //can reload from OTP
    //Power on Setting
    hx8347dWriteReg(0x18, 0x36); //I/P_RADJ,N/P_RADJ, Normal mode 75Hz
    hx8347dWriteReg(0x19, 0x01); //OSC_EN='1', start Osc
    hx8347dWriteReg(0x01, 0x00); //DP_STB='0', out deep sleep
    hx8347dWriteReg(0x1F, 0x88); // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x1F, 0x80); // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x1F, 0x90); // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x1F, 0xD0); // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    LL_mDelay(5);
    //262k/65k color selection
    hx8347dWriteReg(0x17, 0x05); //default 0x06 262k color // 0x05 65k color
    //SET PANEL
    hx8347dWriteReg(0x36, 0x04); //SS_P, GS_P,REV_P,BGR_P
    //Display ON Setting
    hx8347dWriteReg(0x28, 0x38); //GON=1, DTE=1, D=1000
    LL_mDelay(40);
    hx8347dWriteReg(0x28, 0x3C); //GON=1, DTE=1, D=1100
    //Set GRAM Area
    hx8347dWriteReg(0x02, 0x00);
    hx8347dWriteReg(0x03, 0x00); //Column Start
    hx8347dWriteReg(0x04, 0x00);
    hx8347dWriteReg(0x05, 0xEF); //Column End
    hx8347dWriteReg(0x06, 0x00);
    hx8347dWriteReg(0x07, 0x00); //Row Start
    hx8347dWriteReg(0x08, 0x01);
    hx8347dWriteReg(0x09, 0x3F); //Row End

    SET(DISP_CS);
}

void hx8347dInit(DispDriver **driver)
{
    *driver = &drv;
    hx8347dInitSeq();
}

void hx8347dSleep(void)
{
    CLR(DISP_CS);

    // Enter Deep Sleep mode Setting
    hx8347dWriteReg(0x28, 0xB8); //GON=’1’ DTE=’1’ D[1:0]=’10’
    LL_mDelay(40);
    hx8347dWriteReg(0x1F, 0x89); // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=1
    LL_mDelay(40);
    hx8347dWriteReg(0x28, 0x04); //GON=’0’ DTE=’0’ D[1:0]=’01’
    LL_mDelay(40);
    hx8347dWriteReg(0x19, 0x00); //OSC_EN=’0’
    LL_mDelay(5);
    hx8347dWriteReg(0x01, 0xC0); //DP_STB[1:0]=’11’*/

    SET(DISP_CS);
}

void hx8347dWakeup(void)
{
    CLR(DISP_CS);
    // Exit Deep Sleep mode Setting
    hx8347dWriteReg(0x01, 0x00); //DP_STB='0', out deep sleep
    LL_mDelay(10);
    hx8347dWriteReg(0x19, 0x01); //OSC_EN='1', start Osc
    hx8347dWriteReg(0x1F, 0x88); // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x1F, 0x80); // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x1F, 0x90); // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x1F, 0xD0); // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    LL_mDelay(5);
    hx8347dWriteReg(0x28, 0x38); //GON=1, DTE=1, D=1000
    LL_mDelay(40);
    hx8347dWriteReg(0x28, 0x3F); //GON=1, DTE=1, D=1100*/

    SET(DISP_CS);
}

void hx8347dDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    hx8347dSetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void hx8347dDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    hx8347dSetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void hx8347dDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    hx8347dSetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}

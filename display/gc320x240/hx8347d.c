#include "../dispdrv.h"

void hx8347dInit(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    //Driving ability Setting

    dispdrvWriteReg8(0xEA, 0x00); //PTBA[15:8]
    dispdrvWriteReg8(0xEB, 0x20); //PTBA[7:0]
    dispdrvWriteReg8(0xEC, 0x0C); //STBA[15:8]
    dispdrvWriteReg8(0xED, 0xC4); //STBA[7:0]
    dispdrvWriteReg8(0xE8, 0x40); //OPON[7:0]
    dispdrvWriteReg8(0xE9, 0x38); //OPON1[7:0]
    dispdrvWriteReg8(0xF1, 0x01); //OTPS1B
    dispdrvWriteReg8(0xF2, 0x10); //GEN
    dispdrvWriteReg8(0x27, 0xA3); //

    //Gamma 2.2 Setting

    dispdrvWriteReg8(0x40, 0x00); //
    dispdrvWriteReg8(0x41, 0x00); //
    dispdrvWriteReg8(0x42, 0x01); //
    dispdrvWriteReg8(0x43, 0x12); //
    dispdrvWriteReg8(0x44, 0x10); //
    dispdrvWriteReg8(0x45, 0x26); //
    dispdrvWriteReg8(0x46, 0x08); //
    dispdrvWriteReg8(0x47, 0x54); //
    dispdrvWriteReg8(0x48, 0x02); //
    dispdrvWriteReg8(0x49, 0x15); //
    dispdrvWriteReg8(0x4A, 0x19); //
    dispdrvWriteReg8(0x4B, 0x19); //
    dispdrvWriteReg8(0x4C, 0x16); //

    dispdrvWriteReg8(0x50, 0x19); //
    dispdrvWriteReg8(0x51, 0x2F); //
    dispdrvWriteReg8(0x52, 0x2D); //
    dispdrvWriteReg8(0x53, 0x3E); //
    dispdrvWriteReg8(0x54, 0x3F); //
    dispdrvWriteReg8(0x55, 0x3F); //
    dispdrvWriteReg8(0x56, 0x2B); //
    dispdrvWriteReg8(0x57, 0x77); //
    dispdrvWriteReg8(0x58, 0x09); //
    dispdrvWriteReg8(0x59, 0x06); //
    dispdrvWriteReg8(0x5A, 0x06); //
    dispdrvWriteReg8(0x5B, 0x0A); //
    dispdrvWriteReg8(0x5C, 0x1D); //
    dispdrvWriteReg8(0x5D, 0xCC); //

    //Power Voltage Setting

    dispdrvWriteReg8(0x1B, 0x1B); //VRH=4.65V
    dispdrvWriteReg8(0x1A, 0x01); //BT (VGH~15V,VGL~-10V,DDVDH~5V)
    dispdrvWriteReg8(0x24, 0x2F); //VMH(VCOM High voltage ~3.2V)
    dispdrvWriteReg8(0x25, 0x57); //VML(VCOM Low voltage -1.2V)
    //-------------VCOM offset----------
    dispdrvWriteReg8(0x23, 0x79); //for Flicker adjust //can reload from OTP
    //Power on Setting
    dispdrvWriteReg8(0x18, 0x36); //I/P_RADJ,N/P_RADJ, Normal mode 75Hz
    dispdrvWriteReg8(0x19, 0x01); //OSC_EN='1', start Osc
    dispdrvWriteReg8(0x01, 0x00); //DP_STB='0', out deep sleep
    dispdrvWriteReg8(0x1F, 0x88); // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x1F, 0x80); // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x1F, 0x90); // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x1F, 0xD0); // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    utilmDelay(5);
    //262k/65k color selection
    dispdrvWriteReg8(0x17, 0x05); //default 0x06 262k color // 0x05 65k color
    //SET PANEL
    dispdrvWriteReg8(0x36, 0x04); //SS_P, GS_P,REV_P,BGR_P
    //Display ON Setting
    dispdrvWriteReg8(0x28, 0x38); //GON=1, DTE=1, D=1000
    utilmDelay(40);
    dispdrvWriteReg8(0x28, 0x3C); //GON=1, DTE=1, D=1100
    //Set GRAM Area
    dispdrvWriteReg8(0x02, 0x00);
    dispdrvWriteReg8(0x03, 0x00); //Column Start
    dispdrvWriteReg8(0x04, 0x00);
    dispdrvWriteReg8(0x05, 0xEF); //Column End
    dispdrvWriteReg8(0x06, 0x00);
    dispdrvWriteReg8(0x07, 0x00); //Row Start
    dispdrvWriteReg8(0x08, 0x01);
    dispdrvWriteReg8(0x09, 0x3F); //Row End

    SET(DISP_CS);
}

void hx8347dSleep(void)
{
    CLR(DISP_CS);

    // Enter Deep Sleep mode Setting
    dispdrvWriteReg8(0x28, 0xB8); //GON=’1’ DTE=’1’ D[1:0]=’10’
    utilmDelay(40);
    dispdrvWriteReg8(0x1F, 0x89); // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=1
    utilmDelay(40);
    dispdrvWriteReg8(0x28, 0x04); //GON=’0’ DTE=’0’ D[1:0]=’01’
    utilmDelay(40);
    dispdrvWriteReg8(0x19, 0x00); //OSC_EN=’0’
    utilmDelay(5);
    dispdrvWriteReg8(0x01, 0xC0); //DP_STB[1:0]=’11’*/

    SET(DISP_CS);
}

void hx8347dWakeup(void)
{
    CLR(DISP_CS);
    // Exit Deep Sleep mode Setting
    dispdrvWriteReg8(0x01, 0x00); //DP_STB='0', out deep sleep
    utilmDelay(10);
    dispdrvWriteReg8(0x19, 0x01); //OSC_EN='1', start Osc
    dispdrvWriteReg8(0x1F, 0x88); // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x1F, 0x80); // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x1F, 0x90); // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x1F, 0xD0); // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    utilmDelay(5);
    dispdrvWriteReg8(0x28, 0x38); //GON=1, DTE=1, D=1000
    utilmDelay(40);
    dispdrvWriteReg8(0x28, 0x3F); //GON=1, DTE=1, D=1100*/

    SET(DISP_CS);
}

void hx8347dSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg8(0x03, ((y >> 0) & 0xFF));
    dispdrvWriteReg8(0x05, ((y1 >> 0) & 0xFF));

    dispdrvWriteReg8(0x06, ((x >> 8) & 0xFF));
    dispdrvWriteReg8(0x07, ((x >> 0) & 0xFF));
    dispdrvWriteReg8(0x08, ((x1 >> 8) & 0xFF));
    dispdrvWriteReg8(0x09, ((x1 >> 0) & 0xFF));

    dispdrvSelectReg8(0x22);
}

const DispDriver dispdrv = {
    .width = 320,
    .height = 240,
    .init = hx8347dInit,
    .sleep = hx8347dSleep,
    .wakeup = hx8347dWakeup,
    .setWindow = hx8347dSetWindow,
};

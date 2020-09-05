#include "../dispdrv.h"

void hx8347aInit(void)
{
    CLR(DISP_CS);

    // Gamma for CMO 2.8
    dispdrvWriteReg8(0x46, 0x95);
    dispdrvWriteReg8(0x47, 0x51);
    dispdrvWriteReg8(0x48, 0x00);
    dispdrvWriteReg8(0x49, 0x36);
    dispdrvWriteReg8(0x4A, 0x11);
    dispdrvWriteReg8(0x4B, 0x66);
    dispdrvWriteReg8(0x4C, 0x14);
    dispdrvWriteReg8(0x4D, 0x77);
    dispdrvWriteReg8(0x4E, 0x13);
    dispdrvWriteReg8(0x4F, 0x4C);
    dispdrvWriteReg8(0x50, 0x46);
    dispdrvWriteReg8(0x51, 0x46);

    //240x320 window setting
    dispdrvWriteReg8(0x02, 0x00); // Column address start2
    dispdrvWriteReg8(0x03, 0x00); // Column address start1
    dispdrvWriteReg8(0x04, 0x00); // Column address end2
    dispdrvWriteReg8(0x05, 0xEF); // Column address end1
    dispdrvWriteReg8(0x06, 0x00); // Row address start2
    dispdrvWriteReg8(0x07, 0x00); // Row address start1
    dispdrvWriteReg8(0x08, 0x01); // Row address end2
    dispdrvWriteReg8(0x09, 0x3F); // Row address end1

    // Display Setting
    dispdrvWriteReg8(0x01, 0x06); // IDMON=0, INVON=1, NORON=1, PTLON=0
    dispdrvWriteReg8(0x16, 0x48); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0

    dispdrvWriteReg8(0x23, 0x95); // N_DC=1001 0101
    dispdrvWriteReg8(0x24, 0x95); // P_DC=1001 0101
    dispdrvWriteReg8(0x25, 0xFF); // I_DC=1111 1111

    dispdrvWriteReg8(0x27, 0x06); // N_BP=0000 0110
    dispdrvWriteReg8(0x28, 0x06); // N_FP=0000 0110
    dispdrvWriteReg8(0x29, 0x06); // P_BP=0000 0110
    dispdrvWriteReg8(0x2A, 0x06); // P_FP=0000 0110
    dispdrvWriteReg8(0x2C, 0x06); // I_BP=0000 0110
    dispdrvWriteReg8(0x2D, 0x06); // I_FP=0000 0110

    dispdrvWriteReg8(0x3A, 0x01); // N_RTN=0000, N_NW=001
    dispdrvWriteReg8(0x3B, 0x00); // P_RTN=0000, P_NW=000
    dispdrvWriteReg8(0x3C, 0xF0); // I_RTN=1111, I_NW=000
    dispdrvWriteReg8(0x3D, 0x00); // DIV=00
    DISP_MDELAY(20);
    dispdrvWriteReg8(0x35, 0x38); // EQS=38h
    dispdrvWriteReg8(0x36, 0x78); // EQP=78h

    dispdrvWriteReg8(0x3E, 0x38); // SON=38h

    dispdrvWriteReg8(0x40, 0x0F); // GDON=0Fh
    dispdrvWriteReg8(0x41, 0xF0); // GDOFF

    // Power Supply Setting
    dispdrvWriteReg8(0x19, 0x49); // OSCADJ=10 0000, OSD_EN=1 //60Hz
    dispdrvWriteReg8(0x93, 0x0C); // RADJ=1100
    DISP_MDELAY(10);
    dispdrvWriteReg8(0x20, 0x40); // BT=0100

    dispdrvWriteReg8(0x1D, 0x07); // VC1=111
    dispdrvWriteReg8(0x1E, 0x00); // VC3=000
    dispdrvWriteReg8(0x1F, 0x04); // VRH=0100

    // VCOM Setting for CMO 2.8” Panel
    dispdrvWriteReg8(0x44, 0x4D); // VCM=101 0000
    dispdrvWriteReg8(0x45, 0x11); // VDV=1 0001
    DISP_MDELAY(10);

    dispdrvWriteReg8(0x1C, 0x04); // AP=100
    DISP_MDELAY(20);
    dispdrvWriteReg8(0x1B, 0x18); // GASENB=0, PON=1, DK=1, XDK=0, DDVDH_TRI=0, STB=0
    DISP_MDELAY(40);
    dispdrvWriteReg8(0x1B, 0x10); // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    DISP_MDELAY(40);

    dispdrvWriteReg8(0x43, 0x80); //Set VCOMG=1
    DISP_MDELAY(100);

    // Display ON Setting
    dispdrvWriteReg8(0x90, 0x7F); // SAP=0111 1111
    dispdrvWriteReg8(0x26, 0x04); //GON=0, DTE=0, D=01
    DISP_MDELAY(40);
    dispdrvWriteReg8(0x26, 0x24); //GON=1, DTE=0, D=01
    dispdrvWriteReg8(0x26, 0x2C); //GON=1, DTE=0, D=11
    DISP_MDELAY(40);

    dispdrvWriteReg8(0x26, 0x3C); //GON=1, DTE=1, D=11

    // Internal register setting
    dispdrvWriteReg8(0x57, 0x02); //Test_Mode Enable
    dispdrvWriteReg8(0x95, 0x01); // Set Display clock and Pumping clock to synchronize
    dispdrvWriteReg8(0x57, 0x00); // Test_Mode Disable

    SET(DISP_CS);
}

void hx8347aSleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
        // Display Off
        dispdrvWriteReg8(0x26, 0x38); //GON=1, DTE=1, D=10
        DISP_MDELAY(40);
        dispdrvWriteReg8(0x26, 0x28); //GON=1, DTE=0, D=10
        DISP_MDELAY(40);
        dispdrvWriteReg8(0x26, 0x00); //GON=0, DTE=0, D=00
        // Power Off
        dispdrvWriteReg8(0x43, 0x00); // VCOMG=0
        DISP_MDELAY(10);
        dispdrvWriteReg8(0x1B, 0x00); // GASENB=0, PON=0, DK=0, XDK=0,
        // VLCD_TRI=0, STB=0
        DISP_MDELAY(10);
        dispdrvWriteReg8(0x1B, 0x08); // GASENB=0, PON=0, DK=1, XDK=0,
        // VLCD_TRI=0, STB=0
        DISP_MDELAY(10);
        dispdrvWriteReg8(0x1C, 0x00); // AP=000
        DISP_MDELAY(10);
        dispdrvWriteReg8(0x90, 0x00); // SAP=00000000
        DISP_MDELAY(10);
        // Into STB mode
        dispdrvWriteReg8(0x1B, 0x09); // GASSENB=0, PON=0, DK=1, XDK=0,
        // VLCD_TRI=0, STB=1
        DISP_MDELAY(10);
        // Stop Oscillation
        dispdrvWriteReg8(0x19, 0x48); // CADJ=0100, CUADJ=100, OSD_EN=0
    } else {
        // Start Oscillation
        dispdrvWriteReg8(0x0019, 0x0049); // OSCADJ=100 010(FR:60Hz), OSD_EN=1
        DISP_MDELAY(10);

        // Exit STB mode
        dispdrvWriteReg8(0x001B, 0x0008); // NIDSENB=0, PON=0, DK=1, XDK=0,
        // VLCD_TRI=0, STB=0

        // Power Supply Setting
        dispdrvWriteReg8(0x20, 0x40); // BT=0100
        dispdrvWriteReg8(0x1D, 0x07); // VC1=111
        dispdrvWriteReg8(0x1E, 0x00); // VC3=000
        dispdrvWriteReg8(0x1F, 0x03); // VRH=0011
        dispdrvWriteReg8(0x44, 0x20); // VCM=010 0000
        dispdrvWriteReg8(0x45, 0x0E); // VDV=0 1110
        DISP_MDELAY(10);
        dispdrvWriteReg8(0x1C, 0x04); // AP=100
        DISP_MDELAY(20);
        dispdrvWriteReg8(0x1B, 0x18); // NIDSENB=0, PON=1, DK=1, XDK=0,
        DISP_MDELAY(40);

        // VLCD_TRI=0, STB=0
        dispdrvWriteReg8(0x1B, 0x10); // NIDSENB=0, PON=1, DK=0, XDK=0,
        DISP_MDELAY(40);

        // VLCD_TRI=1, STB=0
        dispdrvWriteReg8(0x43, 0x80); // VCOMG=1
        DISP_MDELAY(100);

        // Display ON Setting
        dispdrvWriteReg8(0x90, 0x7F); // SAP=01111111
        DISP_MDELAY(40);
        dispdrvWriteReg8(0x26, 0x04); //GON=0, DTE=0, D=01
        DISP_MDELAY(40);
        dispdrvWriteReg8(0x26, 0x24); //GON=1, DTE=0, D=01
        dispdrvWriteReg8(0x26, 0x2C); //GON=1, DTE=0, D=11
        DISP_MDELAY(40);
        dispdrvWriteReg8(0x26, 0x3C); //GON=1, DTE=1, D=11
    }

    SET(DISP_CS);
}

void hx8347aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .init = hx8347aInit,
    .sleep = hx8347aSleep,
    .setWindow = hx8347aSetWindow,
};

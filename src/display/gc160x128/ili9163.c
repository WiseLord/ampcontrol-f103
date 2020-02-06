#include "../dispdrv.h"

void ili9163Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    //************* Start Initial Sequence **********//
    dispdrvSelectReg8(0x11); //Exit Sleep
    utilmDelay(20);

    dispdrvSelectReg8(0x26); //Set Default Gamma
    dispdrvSendData8(0x04);

    dispdrvSelectReg8(0xB1);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x14);

    dispdrvSelectReg8(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x05);

    dispdrvSelectReg8(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    dispdrvSendData8(0x02);

    dispdrvSelectReg8(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    dispdrvSendData8(0x29);
    dispdrvSendData8(0x43);

    dispdrvSelectReg8(0xC7);
    dispdrvSendData8(0x40);

    dispdrvSelectReg8(0x3a); //Set Color Format
    dispdrvSendData8(0x05);

    dispdrvSelectReg8(0x2A); //Set Column Address
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x7F);

    dispdrvSelectReg8(0x2B); //Set Page Address
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x9F);

    dispdrvSelectReg8(0x36); //Set Scanning Direction
    dispdrvSendData8(0x88);

    dispdrvSelectReg8(0xB7); //Set Source Output Direction
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xf2); //Enable Gamma bit
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0xE0);
    dispdrvSendData8(0x36);//p1
    dispdrvSendData8(0x29);//p2
    dispdrvSendData8(0x12);//p3
    dispdrvSendData8(0x22);//p4
    dispdrvSendData8(0x1C);//p5
    dispdrvSendData8(0x15);//p6
    dispdrvSendData8(0x42);//p7
    dispdrvSendData8(0xB7);//p8
    dispdrvSendData8(0x2F);//p9
    dispdrvSendData8(0x13);//p10
    dispdrvSendData8(0x12);//p11
    dispdrvSendData8(0x0A);//p12
    dispdrvSendData8(0x11);//p13
    dispdrvSendData8(0x0B);//p14
    dispdrvSendData8(0x06);//p15

    dispdrvSelectReg8(0xE1);
    dispdrvSendData8(0x09);//p1
    dispdrvSendData8(0x16);//p2
    dispdrvSendData8(0x2D);//p3
    dispdrvSendData8(0x0D);//p4
    dispdrvSendData8(0x13);//p5
    dispdrvSendData8(0x15);//p6
    dispdrvSendData8(0x40);//p7
    dispdrvSendData8(0x48);//p8
    dispdrvSendData8(0x53);//p9
    dispdrvSendData8(0x0C);//p10
    dispdrvSendData8(0x1D);//p11
    dispdrvSendData8(0x25);//p12
    dispdrvSendData8(0x2E);//p13
    dispdrvSendData8(0x34);//p14
    dispdrvSendData8(0x39);//p15

    dispdrvSelectReg8(0x29); // Display On

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9163Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x28);    // Display OFF
    utilmDelay(100);
    dispdrvSelectReg8(0x10);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9163Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x11);    // Display OFF
    utilmDelay(100);
    dispdrvSelectReg8(0x29);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9163SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .width = 162,
    .height = 132,
    .sleep = ili9163Sleep,
    .wakeup = ili9163Wakeup,
    .init = ili9163Init,
    .setWindow = ili9163SetWindow,
};

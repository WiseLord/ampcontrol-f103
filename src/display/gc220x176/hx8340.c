#include "../dispdrv.h"

void hx8340Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    //Driving ability Setting
    dispdrvWriteReg8(0x60, 0x00); //PTBA[15:8]
    dispdrvWriteReg8(0x61, 0x06); //PTBA[15:8]
    dispdrvWriteReg8(0x62, 0x00); //STBA[15:8]
    dispdrvWriteReg8(0x63, 0xC8); //STBA[7:0]
    dispdrvWriteReg8(0x73, 0x70); //OPON[7:0],SET OPON=70h, default 38h

    //Gamma 2.2 Setting
    dispdrvWriteReg8(0x40, 0x00);
    dispdrvWriteReg8(0x41, 0x40);
    dispdrvWriteReg8(0x42, 0x45);
    dispdrvWriteReg8(0x43, 0x01);
    dispdrvWriteReg8(0x44, 0x60);
    dispdrvWriteReg8(0x45, 0x05);
    dispdrvWriteReg8(0x46, 0x0C);
    dispdrvWriteReg8(0x47, 0xD1);
    dispdrvWriteReg8(0x48, 0x05);
    dispdrvWriteReg8(0x50, 0x75);
    dispdrvWriteReg8(0x51, 0x01);
    dispdrvWriteReg8(0x52, 0x67);
    dispdrvWriteReg8(0x53, 0x14);
    dispdrvWriteReg8(0x54, 0xF2);
    dispdrvWriteReg8(0x55, 0x07);
    dispdrvWriteReg8(0x56, 0x03);
    dispdrvWriteReg8(0x57, 0x49);

    //Power Voltage Setting
    dispdrvWriteReg8(0x1F, 0x03); //VRH=4.65V
    dispdrvWriteReg8(0x20, 0x00); //BT (VGH~15V,VGL~-12V,DDVDH~5V)
    dispdrvWriteReg8(0x24, 0x1C); //VMH(VCOM High voltage3.2V)
    dispdrvWriteReg8(0x25, 0x34); //VML(VCOM Low voltage -1.2V)

    //****VCOM offset**///
    dispdrvWriteReg8(0x23, 0x2F);

    //Power on Setting
    dispdrvWriteReg8(0x18, 0x44); //I/P_RADJ,N/P_RADJ, Normal mode 60Hz
    dispdrvWriteReg8(0x21, 0x01); //OSC_EN='1', start Osc
    dispdrvWriteReg8(0x01, 0x00); //SLP='0', out sleep
    dispdrvWriteReg8(0x1C, 0x03); //AP=011
    dispdrvWriteReg8(0x19, 0x06); // VOMG=1,PON=1, DK=0,
    utilmDelay(5);

    //Display ON Setting
    dispdrvWriteReg8(0x26, 0x84); //PT=10,GON=0, DTE=0, D=0100
    utilmDelay(40);
    dispdrvWriteReg8(0x26, 0xB8); //PT=10,GON=1, DTE=1, D=1000
    utilmDelay(40);
    dispdrvWriteReg8(0x26, 0xBC); //PT=10,GON=1, DTE=1, D=1100

    //Set GRAM Area
    dispdrvWriteReg8(0x02, 0x00); //Column Start
    dispdrvWriteReg8(0x03, 0x00);
    dispdrvWriteReg8(0x04, 0x00); //Column End
    dispdrvWriteReg8(0x05, 0xAF);
    dispdrvWriteReg8(0x06, 0x00); //Row Start
    dispdrvWriteReg8(0x07, 0x00);
    dispdrvWriteReg8(0x08, 0x00); //Row End
    dispdrvWriteReg8(0x09, 0xDB);

    dispdrvWriteReg8(0x17, 0x05);

    dispdrvWriteReg8(0x16, 0x48);

    SET(DISP_CS);
}

void hx8340Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg8(0x26, 0xB8); //GON=’1’ DTE=’1’ D[1:0]=’10’
    utilmDelay(40);
    dispdrvWriteReg8(0x19, 0x01); //VCOMG=’0’, PON=’0’, DK=’1’
    utilmDelay(40);
    dispdrvWriteReg8(0x26, 0xA4); //GON=’1’ DTE=’0’ D[1:0]=’01’
    utilmDelay(40);
    dispdrvWriteReg8(0x26, 0x84); //GON=’0’ DTE=’0’ D[1:0]=’01’
    utilmDelay(40);
    dispdrvWriteReg8(0x1C, 0x00); //AP[2:0]=’000’
    dispdrvWriteReg8(0x01, 0x02); //SLP=’1’
    dispdrvWriteReg8(0x01, 0x00); //OSC_EN=’0’

    SET(DISP_CS);
}

void hx8340Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg8(0x18, 0x44); //I/P_RADJ,N/P_RADJ, Normal mode 60Hz
    dispdrvWriteReg8(0x21, 0x01); //OSC_EN='1', start Osc
    dispdrvWriteReg8(0x01, 0x00); //SLP='0', out sleep
    dispdrvWriteReg8(0x1C, 0x03); //AP=011
    dispdrvWriteReg8(0x19, 0x06); // VOMG=1,PON=1, DK=0,
    utilmDelay(5);
    dispdrvWriteReg8(0x26, 0x84); //PT=10,GON=0, DTE=0, D=0100
    utilmDelay(40);
    dispdrvWriteReg8(0x26, 0xB8); //PT=10,GON=1, DTE=1, D=1000
    utilmDelay(40);
    dispdrvWriteReg8(0x26, 0xBC); //PT=10,GON=1, DTE=1, D=1100

    SET(DISP_CS);
}

void hx8340SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    uint8_t x0 = (uint8_t)x;
    uint8_t y0 = (uint8_t)y;
    uint8_t x1 = (uint8_t)(x + w - 1);
    uint8_t y1 = (uint8_t)(y + h - 1);

    dispdrvWriteReg8(0x03, y0);
    dispdrvWriteReg8(0x05, y1);

    dispdrvWriteReg8(0x07, x0);
    dispdrvWriteReg8(0x09, x1);

    dispdrvSelectReg8(0x22);
}

const DispDriver dispdrv = {
    .width = 220,
    .height = 176,
    .init = hx8340Init,
    .sleep = hx8340Sleep,
    .wakeup = hx8340Wakeup,
    .setWindow = hx8340SetWindow,
};

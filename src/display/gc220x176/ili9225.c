#include "../dispdrv.h"

void ili9225Init(void)
{
    CLR(DISP_CS);

    // Start Initial Sequence
    dispdrvWriteReg16(0x0001, 0x001C);    // set SS and NL bit
    dispdrvWriteReg16(0x0002, 0x0100);    // set 1 line inversion
    dispdrvWriteReg16(0x0003, 0x1030);    // set GRAM write direction and BGR=1.
    dispdrvWriteReg16(0x0008, 0x0808);    // set BP and FP
    dispdrvWriteReg16(0x000C, 0x0000);    // RGB interface setting
    dispdrvWriteReg16(0x000F, 0x0801);    // Set frame rate

    // Power On sequence

    utilmDelay(50); // Delay 50ms

    dispdrvWriteReg16(0x0010, 0x0A00);    // Set SAP,DSTB,STB
    dispdrvWriteReg16(0x0011, 0x1038);    // Set APON,PON,AON,VCI1EN,VC

    utilmDelay(50); // Delay 50ms

    dispdrvWriteReg16(0x0012, 0x6121);    // Internal reference voltage= Vci;
    dispdrvWriteReg16(0x0013, 0x0062);    // Set GVDD
    dispdrvWriteReg16(0x0014, 0x5b60);    // Set VCOMH/VCOML voltage

    // Set GRAM area
    dispdrvWriteReg16(0x0030, 0x0000);
    dispdrvWriteReg16(0x0031, 0x00DB);
    dispdrvWriteReg16(0x0032, 0x0000);
    dispdrvWriteReg16(0x0033, 0x0000);
    dispdrvWriteReg16(0x0034, 0x00DB);
    dispdrvWriteReg16(0x0035, 0x0000);
    dispdrvWriteReg16(0x0036, 0x00AF);
    dispdrvWriteReg16(0x0037, 0x0000);
    dispdrvWriteReg16(0x0038, 0x00DB);
    dispdrvWriteReg16(0x0039, 0x0000);
    dispdrvWriteReg16(0x0020, 0x0000);    // Set GRAM Address
    dispdrvWriteReg16(0x0021, 0x0000);    // Set GRAM Address

    // Adjust the Gamma  Curve
    dispdrvWriteReg16(0x0050, 0x0000);
    dispdrvWriteReg16(0x0051, 0x000B);
    dispdrvWriteReg16(0x0052, 0x0a01);
    dispdrvWriteReg16(0x0053, 0x010c);
    dispdrvWriteReg16(0x0054, 0x010a);
    dispdrvWriteReg16(0x0055, 0x0B00);
    dispdrvWriteReg16(0x0056, 0x0000);
    dispdrvWriteReg16(0x0057, 0x0c01);
    dispdrvWriteReg16(0x0058, 0x0E00);
    dispdrvWriteReg16(0x0059, 0x000E);

    utilmDelay(50); // Delay 50ms

    dispdrvWriteReg16(0x0007, 0x1017);  // 65K color and display ON*/

    SET(DISP_CS);
}

void ili9225Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);    // Display OFF
    utilmDelay(50);
    dispdrvWriteReg16(0x0010, 0x0A01);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void ili9225Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0010, 0x0A00);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    utilmDelay(50);
    dispdrvWriteReg16(0x0007, 0x1017);    // 65K color and display ON

    SET(DISP_CS);
}

void ili9225SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0036, (uint16_t)y1);
    dispdrvWriteReg16(0x0037, (uint16_t)y);
    dispdrvWriteReg16(0x0038, (uint16_t)x1);
    dispdrvWriteReg16(0x0039, (uint16_t)x);

    // Set cursor
    dispdrvWriteReg16(0x0020, (uint16_t)y);
    dispdrvWriteReg16(0x0021, (uint16_t)x);

    // Select RAM mode
    dispdrvSelectReg16(0x0022);
}

const DispDriver dispdrv = {
    .width = 220,
    .height = 176,
    .init = ili9225Init,
    .sleep = ili9225Sleep,
    .wakeup = ili9225Wakeup,
    .setWindow = ili9225SetWindow,
};

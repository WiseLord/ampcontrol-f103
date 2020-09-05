#include "../dispdrv.h"

void s6d0164Init(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0011, 0x001A);
    dispdrvWriteReg16(0x0012, 0x3121);
    dispdrvWriteReg16(0x0013, 0x006C);
    dispdrvWriteReg16(0x0014, 0x4249);

    dispdrvWriteReg16(0x0010, 0x0800);
    DISP_MDELAY(10);
    dispdrvWriteReg16(0x0011, 0x011A);
    DISP_MDELAY(10);
    dispdrvWriteReg16(0x0011, 0x031A);
    DISP_MDELAY(10);
    dispdrvWriteReg16(0x0011, 0x071A);
    DISP_MDELAY(10);
    dispdrvWriteReg16(0x0011, 0x0F1A);
    DISP_MDELAY(10);
    dispdrvWriteReg16(0x0011, 0x0F3A);
    DISP_MDELAY(30);

    dispdrvWriteReg16(0x0001, 0x001C);
    dispdrvWriteReg16(0x0002, 0x0100);
    dispdrvWriteReg16(0x0003, 0x1030);
    dispdrvWriteReg16(0x0007, 0x0000);
    dispdrvWriteReg16(0x0008, 0x0808);

    dispdrvWriteReg16(0x0009, 0x0000);

    dispdrvWriteReg16(0x000B, 0x1100);
    dispdrvWriteReg16(0x000C, 0x0000);
    dispdrvWriteReg16(0x000F, 0x1401);
    dispdrvWriteReg16(0x0015, 0x0000);

    dispdrvWriteReg16(0x0050, 0x0001);
    dispdrvWriteReg16(0x0051, 0x020B);
    dispdrvWriteReg16(0x0052, 0x0805);
    dispdrvWriteReg16(0x0053, 0x0404);
    dispdrvWriteReg16(0x0054, 0x0C0C);
    dispdrvWriteReg16(0x0055, 0x000C);
    dispdrvWriteReg16(0x0056, 0x0101);
    dispdrvWriteReg16(0x0057, 0x0400);
    dispdrvWriteReg16(0x0058, 0x1108);
    dispdrvWriteReg16(0x0059, 0x050C);

    dispdrvWriteReg16(0x0020, 0x0000);
    dispdrvWriteReg16(0x0021, 0x0000);

    dispdrvWriteReg16(0x0036, 0x00AF);
    dispdrvWriteReg16(0x0037, 0x0000);
    dispdrvWriteReg16(0x0038, 0x00DB);
    dispdrvWriteReg16(0x0039, 0x0000);

    dispdrvWriteReg16(0x000F, 0x0B01);
    dispdrvWriteReg16(0x0007, 0x0016);
    DISP_MDELAY(10);
    dispdrvWriteReg16(0x0007, 0x0017);

    SET(DISP_CS);
}

void s6d0164Sleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
        dispdrvWriteReg16(0x0007, 0x0000);    // Display OFF
        DISP_MDELAY(50);
        dispdrvWriteReg16(0x0010, 0x0A01);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    } else {
        dispdrvWriteReg16(0x0010, 0x0A00);    // SAP, BT[3:0], AP, DSTB, SLP, STB
        DISP_MDELAY(50);
        dispdrvWriteReg16(0x0007, 0x1017);    // 65K color and display ON
    }

    SET(DISP_CS);
}

void s6d0164SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .init = s6d0164Init,
    .sleep = s6d0164Sleep,
    .setWindow = s6d0164SetWindow,
};

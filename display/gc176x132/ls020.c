#include "../dispdrv.h"

void ls020Init(void)
{
    CLR(DISP_CS);

    dispdrvSendData16(0xFDFD);
    dispdrvSendData16(0xFDFD);
    utilmDelay(50);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0xEE04);
    dispdrvSendData16(0x1B04);
    dispdrvSendData16(0xFEFE);
    dispdrvSendData16(0xFEFE);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x4A04);
    dispdrvSendData16(0x7F3F);
    dispdrvSendData16(0xEE04);
    dispdrvSendData16(0x4306);
    utilmDelay(50);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x0983);
    dispdrvSendData16(0x0800);
    dispdrvSendData16(0x0BAF);
    dispdrvSendData16(0x0A00);
    dispdrvSendData16(0x0500);
    dispdrvSendData16(0x0600);
    dispdrvSendData16(0x0700);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0xEE0C);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x0080);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0x4902);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xE181);
    dispdrvSendData16(0xE202);
    dispdrvSendData16(0xE276);
    dispdrvSendData16(0xE183);
    utilmDelay(50);
    dispdrvSendData16(0x8001);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x0000); // Mirror? 0x0020

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ls020Sleep(void)
{
    SET(DISP_RS);
    CLR(DISP_CS);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7E04);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0x5A48);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0x64FF);
    dispdrvSendData16(0x6500);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xE262);
    dispdrvSendData16(0xE202);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0xBC02);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xE200);
    dispdrvSendData16(0x8000);
    dispdrvSendData16(0xE204);
    dispdrvSendData16(0xE200);
    dispdrvSendData16(0xE100);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0xBC00);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ls020Wakeup(void)
{
    ls020Init();
}

void ls020SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    SET(DISP_RS);
    CLR(DISP_CS);

    dispdrvSendData16((uint16_t)(0x0800 + y));
    dispdrvSendData16((uint16_t)(0x0900 + y1));

    dispdrvSendData16((uint16_t)(0x0A00 + x));
    dispdrvSendData16((uint16_t)(0x0B00 + x1));

    DISP_WAIT_BUSY();
    SET(DISP_CS);
    CLR(DISP_RS);
}

const DispDriver dispdrv = {
    .width = 176,
    .height = 132,
    .init = ls020Init,
    .sleep = ls020Sleep,
    .wakeup = ls020Wakeup,
    .setWindow = ls020SetWindow,
};

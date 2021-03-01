#include "../dispdrv.h"

void mc2pa8201Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    // Wait for reset
    DISP_MDELAY(50);

    CLR(DISP_CS);

    dispdrvSelectReg8(0x01);
    DISP_MDELAY(100);

    dispdrvSelectReg8(0x11);
    DISP_MDELAY(100);

    dispdrvSelectReg8(0x20);

    dispdrvSelectReg8(0x26); //Set Default Gamma
    dispdrvSendData8(0x04);

    dispdrvSelectReg8(0x3A);
    dispdrvSendData8(0x05);

    dispdrvSelectReg8(0x2d);

    for (uint8_t r1 = 0; r1 < 32; r1++)
        dispdrvSendData8((uint8_t)(r1 << 3));
    for (uint8_t r2 = 0; r2 < 32; r2++)
        dispdrvSendData8((r2));
    for (uint8_t g1 = 0; g1 < 64; g1++)
        dispdrvSendData8((uint8_t)(g1 << 2));
    for (uint8_t b1 = 0; b1 < 32; b1++)
        dispdrvSendData8((uint8_t)(b1 << 3));
    for (uint8_t b2 = 0; b2 < 32; b2++)
        dispdrvSendData8(0);

//    mc2pa8201SelectReg(0x51);
//    dispdrvSendData8(0xff);

//    mc2pa8201SelectReg(0x53);
//    dispdrvSendData8(0x24);

    dispdrvSelectReg8(0x36);
    dispdrvSendData8(0x20); // 0x80 for vertical layout

    dispdrvSelectReg8(0x29);

    SET(DISP_CS);
}

void mc2pa8201Rotate(bool rotate)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x36);

    if (rotate) {
        dispdrvSendData8(0xE0);
    } else {
        dispdrvSendData8(0x20);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void mc2pa8201Sleep(bool value)
{
    CLR(DISP_CS);

    if (value) {
        dispdrvSelectReg8(0x28);    // Display OFF
        DISP_MDELAY(100);
        dispdrvSelectReg8(0x10);
    } else {
        dispdrvSelectReg8(0x11);    // Display OFF
        DISP_MDELAY(100);
        dispdrvSelectReg8(0x29);
    }

    SET(DISP_CS);
}

void mc2pa8201SetIdle(bool value)
{
    CLR(DISP_CS);

    if (value) {
        dispdrvSelectReg8(0x39);    // 8-color idle mode on
    } else {
        dispdrvSelectReg8(0x38);    // 8-color idle mode off
    }

    SET(DISP_CS);
}

void mc2pa8201SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
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
    .width = 320,
    .height = 240,
    .init = mc2pa8201Init,
    .sleep = mc2pa8201Sleep,
    .setIdle = mc2pa8201SetIdle,
    .setWindow = mc2pa8201SetWindow,
    .rotate = mc2pa8201Rotate,
};

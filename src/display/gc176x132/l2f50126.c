#include "../dispdrv.h"

static const uint8_t disctl[9] = {
    0x4C, 0x01, 0x53, 0x00, 0x02, 0xB4, 0xB0, 0x02, 0x00,
};

static const uint8_t gcp64_0[29] = {
    0x11, 0x27, 0x3C, 0x4C, 0x5D, 0x6C, 0x78, 0x84, 0x90, 0x99,
    0xA2, 0xAA, 0xB2, 0xBA, 0xC0, 0xC7, 0xCC, 0xD2, 0xD7, 0xDC,
    0xE0, 0xE4, 0xE8, 0xED, 0xF0, 0xF4, 0xF7, 0xFB, 0xFE,
};

static const uint8_t gcp64_1[34] = {
    0x01, 0x03, 0x06, 0x09, 0x0B, 0x0E, 0x10, 0x13, 0x15, 0x17,
    0x19, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C,
    0x2D, 0x2F, 0x31, 0x33, 0x35, 0x37, 0x39, 0x3B, 0x3D, 0x3F,
    0x42, 0x44, 0x47, 0x5E,
};
static const uint8_t gcp16[15] = {
    0x13, 0x23, 0x2D, 0x33, 0x38, 0x3C, 0x40, 0x43, 0x46, 0x48,
    0x4A, 0x4C, 0x4E, 0x50, 0x64,
};

__attribute__((always_inline))
static inline void l2f50126SendData0(uint8_t data)
{
    dispdrvSendData8(data);
    dispdrvSendData8(0x00);
}

__attribute__((always_inline))
static inline void l2f50126SelectReg(uint8_t cmd)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    l2f50126SendData0(cmd);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

/*
__attribute__((always_inline))
static void inline l2f50126PulseCS(void)
{
    DISP_WAIT_BUSY();
    SET(DISP_CS);
    utilmDelay(1);
    CLR(DISP_CS);
}
*/

void l2f50126Init(void)
{
    CLR(DISP_CS);

    l2f50126SelectReg(0xBC); // Data Control (data handling in RAM)
    l2f50126SendData0(0x2B);

    // l2f50126PulseCS();

    l2f50126SelectReg(0xCA); // Display Control
    for (uint8_t i = 0; i < sizeof(disctl); i++) {
        l2f50126SendData0(disctl[i]);
    }

    l2f50126SelectReg(0xCB); // pulse set for 64 gray scale
    for (uint8_t i = 0; i < sizeof(gcp64_0); i++) {
        l2f50126SendData0(gcp64_0[i]);
        l2f50126SendData0(0x00);
    }
    for (uint8_t i = 0; i < sizeof(gcp64_1); i++) {
        l2f50126SendData0(gcp64_1[i]);
        l2f50126SendData0(0x01);
    }

    l2f50126SelectReg(0xCC); // pulse set for 16 gray scale
    for (uint8_t i = 0; i < sizeof(gcp16); i++) {
        l2f50126SendData0(gcp16[i]);
    }

    l2f50126SelectReg(0xCD); // set for gray scales
    l2f50126SendData0(0x00);

    l2f50126SelectReg(0xD0); // Oscillator select
    l2f50126SendData0(0x00);

    l2f50126SelectReg(0x94); // Display out of sleep (no parameter)

    l2f50126SelectReg(0x15); // column address setting
    l2f50126SendData0(0x08);
    l2f50126SendData0(0x01);
    l2f50126SendData0(0x8B);
    l2f50126SendData0(0x01);

    l2f50126SelectReg(0x75); // page address setting
    l2f50126SendData0(0x00);
    l2f50126SendData0(0x8F);

    l2f50126SelectReg(0xAA); // area scroll setting
    l2f50126SendData0(0x00);
    l2f50126SendData0(0xAF);
    l2f50126SendData0(0xAF);
    l2f50126SendData0(0x03);

    l2f50126SelectReg(0xAB); // scroll start setting
    l2f50126SendData0(0x00);

    l2f50126SelectReg(0xAF); // Display ON (no parameter)

    // Additional display registers:
    // l2f50126SelectReg(0xCD); // set for gray scales
    // l2f50126SelectReg(0xA7); // Display Invert (no parameter)
    // l2f50126SelectReg(0xA6); // Display Normal (no parameter)
    // l2f50126SelectReg(0xA8); // partial screen write

    SET(DISP_CS);
}

void l2f50126Sleep(void)
{
    CLR(DISP_CS);

    l2f50126SelectReg(0x95); // Display Sleep (no parameter)
    l2f50126SelectReg(0xAE); // Display OFF (no parameter)

    SET(DISP_CS);

}

void l2f50126Wakeup(void)
{
    CLR(DISP_CS);

    l2f50126SelectReg(0x94); // Display out of sleep (no parameter)
    l2f50126SelectReg(0xAF); // Display ON (no parameter)

    SET(DISP_CS);
}

void l2f50126SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    uint8_t x0 = (uint8_t)x;
    uint8_t y0 = (uint8_t)y + 8;
    uint8_t x1 = (uint8_t)(x + w - 1);
    uint8_t y1 = (uint8_t)(y + 8 + h - 1);

    l2f50126SelectReg(0x15); // column address setting
    l2f50126SendData0(y0);
    l2f50126SendData0(0x01);
    l2f50126SendData0(y1);
    l2f50126SendData0(0x01);

    l2f50126SelectReg(0x75); // page address setting
    l2f50126SendData0(x0);
    l2f50126SendData0(x1);

    l2f50126SelectReg(0x5C); // Display Memory write
}

const DispDriver dispdrv = {
    .width = 176,
    .height = 132,
    .init = l2f50126Init,
    .sleep = l2f50126Sleep,
    .wakeup = l2f50126Wakeup,
    .setWindow = l2f50126SetWindow,
};

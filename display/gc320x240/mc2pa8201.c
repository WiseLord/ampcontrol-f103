#include "mc2pa8201.h"

#include "../dispdrv.h"
#include "../../pins.h"
#include "../../functions.h"

#define MC2PA8201_WIDTH           240
#define MC2PA8201_HEIGHT          320
#define MC2PA8201_PIXELS          (MC2PA8201_WIDTH * MC2PA8201_HEIGHT)

static GlcdDriver glcd = {
    .clear = mc2pa8201Clear,
    .drawPixel = mc2pa8201DrawPixel,
    .drawRectangle = mc2pa8201DrawRectangle,
    .drawImage = mc2pa8201DrawImage,
};

static inline void mc2pa8201SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void mc2pa8201SelectReg(uint8_t reg)
{
    CLR(DISP_8BIT_RS);
    dispdrvSendData8(reg);
    SET(DISP_8BIT_RS);
}

static inline void mc2pa8201InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(DISP_8BIT_CS);

    // Initial Sequence
    // Wait for reset
    _delay_ms(50);

    CLR(DISP_8BIT_CS);

    mc2pa8201SelectReg(0x01);
    _delay_ms(100);

    mc2pa8201SelectReg(0x11);
    _delay_ms(100);

    mc2pa8201SelectReg(0x20);

    mc2pa8201SelectReg(0x26); //Set Default Gamma
    dispdrvSendData8(0x04);

    mc2pa8201SelectReg(0x3A);
    dispdrvSendData8(0x05);

    mc2pa8201SelectReg(0x2d);

    for (uint8_t r1 = 0; r1 < 32; r1++)
        dispdrvSendData8(r1 << 3);
    for (uint8_t rs2 = 0; rs2 < 32; rs2++)
        dispdrvSendData8(rs2);
    for (uint8_t g1 = 0; g1 < 64; g1++)
        dispdrvSendData8(g1 << 2);
    for (uint8_t b1 = 0; b1 < 32; b1++)
        dispdrvSendData8(b1 << 3);
    for (uint8_t b2 = 0; b2 < 32; b2++)
        dispdrvSendData8(0);
    /*
    mc2pa8201SelectReg(0x51);
    dispdrvSendData8(0xff);

    mc2pa8201SelectReg(0x53);
    dispdrvSendData8(0x24);
    */
    mc2pa8201SelectReg(0x36);
    dispdrvSendData8(0x80);

    mc2pa8201SelectReg(0x29);

    SET(DISP_8BIT_CS);
}

static inline void mc2pa8201SetWindow(uint16_t x, uint16_t y, uint16_t w,
                                      uint16_t h) __attribute__((always_inline));
static inline void mc2pa8201SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    mc2pa8201SelectReg(0x2A);
    dispdrvSendData16(y);
    dispdrvSendData16(y + h - 1);

    mc2pa8201SelectReg(0x2B);
    dispdrvSendData16(x);
    dispdrvSendData16(x + w - 1);

    mc2pa8201SelectReg(0x2C);
}

void mc2pa8201Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc320x240Init(*driver);

    SET(DISP_8BIT_LED);
    SET(DISP_8BIT_RD);
    SET(DISP_8BIT_WR);
    SET(DISP_8BIT_RS);
    SET(DISP_8BIT_CS);

    CLR(DISP_8BIT_RST);
    _delay_ms(1);
    SET(DISP_8BIT_RST);

    mc2pa8201InitSeq();
}

void mc2pa8201Clear(void)
{
    mc2pa8201DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void mc2pa8201Sleep(void)
{
    CLR(DISP_8BIT_CS);

    mc2pa8201SelectReg(0x28);    // Display OFF
    _delay_ms(100);
    mc2pa8201SelectReg(0x10);

    SET(DISP_8BIT_CS);
}

void mc2pa8201Wakeup(void)
{
    CLR(DISP_8BIT_CS);

    mc2pa8201SelectReg(0x11);    // Display OFF
    _delay_ms(100);
    mc2pa8201SelectReg(0x29);

    SET(DISP_8BIT_CS);
}

void mc2pa8201DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    mc2pa8201SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_8BIT_CS);
}

void mc2pa8201DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    mc2pa8201SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_8BIT_CS);
}

void mc2pa8201DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(DISP_8BIT_CS);

    mc2pa8201SetWindow(x0, y0, w, h);
    dispdrvSendImage(img, w, h);

    SET(DISP_8BIT_CS);
}

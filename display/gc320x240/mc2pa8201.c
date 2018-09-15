#include "mc2pa8201.h"

#include "../dispcanvas.h"

#include "../../pins.h"
#include "../../functions.h"

#define MC2PA8201_WIDTH           240
#define MC2PA8201_HEIGHT          320
#define MC2PA8201_PIXELS          (MC2PA8201_WIDTH * MC2PA8201_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = mc2pa8201Clear,
    .drawPixel = mc2pa8201DrawPixel,
    .drawRectangle = mc2pa8201DrawRectangle,
    .drawImage = mc2pa8201DrawImage,
};

static inline void mc2pa8201SendData(uint16_t data) __attribute__((always_inline));
static inline void mc2pa8201SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    DISP_8BIT_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(DISP_8BIT_WR);                                // Strob MSB
    SET(DISP_8BIT_WR);
    DISP_8BIT_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(DISP_8BIT_WR);                                // Strob LSB
    SET(DISP_8BIT_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        DISP_8BIT_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        DISP_8BIT_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = DISP_8BIT_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        DISP_8BIT_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void mc2pa8201SendDataR(uint8_t dataR) __attribute__((always_inline));
static inline void mc2pa8201SendDataR(uint8_t dataR)
{
    DISP_8BIT_DHI_Port->BSRR = 0x00FF0000 | dataR;    // If port bits 7..0 are used
    CLR(DISP_8BIT_WR);                                // Strob LSB
    SET(DISP_8BIT_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        DISP_8BIT_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        DISP_8BIT_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = DISP_8BIT_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        DISP_8BIT_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}


static inline void mc2pa8201SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void mc2pa8201SelectReg(uint8_t reg)
{
    CLR(DISP_8BIT_RS);
    mc2pa8201SendDataR(reg);
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
        mc2pa8201SendDataR(0x04);

        mc2pa8201SelectReg(0x3A);
        mc2pa8201SendDataR(0x05);

        mc2pa8201SelectReg(0x2d);

        for (uint8_t r1 = 0; r1 < 32; r1++)
            mc2pa8201SendDataR(r1 << 3);
        for (uint8_t r2 = 0; r2 < 32; r2++)
            mc2pa8201SendDataR(r2);
        for (uint8_t g1 = 0; g1 < 64; g1++)
            mc2pa8201SendDataR(g1 << 2);
        for (uint8_t b1 = 0; b1 < 32; b1++)
            mc2pa8201SendDataR(b1 << 3);
        for (uint8_t b2 = 0; b2 < 32; b2++)
            mc2pa8201SendDataR(0);


     /* mc2pa8201SelectReg(0x51);
        mc2pa8201SendDataR(0xff);

        mc2pa8201SelectReg(0x53);
        mc2pa8201SendDataR(0x24); */

        mc2pa8201SelectReg(0x36);
        mc2pa8201SendDataR(0x80);

        mc2pa8201SelectReg(0x29);

    SET(DISP_8BIT_CS);
}

static inline void mc2pa8201SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void mc2pa8201SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

    mc2pa8201SelectReg(0x2A);
    mc2pa8201SendData(y);
    mc2pa8201SendData(y + h - 1);
    mc2pa8201SelectReg(0x2B);
    mc2pa8201SendData(x);
    mc2pa8201SendData(x + w - 1);
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

void mc2pa8201BusIRQ(void)
{
    bus_requested = 1;
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

    mc2pa8201SelectReg(0x2C);
    mc2pa8201SendData(color);

    SET(DISP_8BIT_CS);
}

void mc2pa8201DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    mc2pa8201SetWindow(x, y, w, h);

    mc2pa8201SelectReg(0x2C);
    for (uint32_t i = 0; i < w * h; i++)
        mc2pa8201SendData(color);

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

    mc2pa8201SelectReg(0x2C);
    
    DISPDRV_SEND_IMAGE(img, mc2pa8201SendData);

    SET(DISP_8BIT_CS);
}

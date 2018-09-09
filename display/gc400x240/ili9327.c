#include "ili9327.h"

#include "../../pins.h"
#include "../../functions.h"

#define ILI9327_WIDTH           240
#define ILI9327_HEIGHT          400
#define ILI9327_PIXELS          (ILI9327_WIDTH * ILI9327_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = ili9327Clear,
    .drawPixel = ili9327DrawPixel,
    .drawRectangle = ili9327DrawRectangle,
    .drawImage = ili9327DrawImage,
};

static inline void ili9327SendData(uint16_t data) __attribute__((always_inline));
static inline void ili9327SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ILI9327_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(ILI9327_WR);                                // Strob MSB
    SET(ILI9327_WR);
    ILI9327_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(ILI9327_WR);                                // Strob LSB
    SET(ILI9327_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ILI9327_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ILI9327_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ILI9327_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ILI9327_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void ili9327SendDataR(uint8_t dataR) __attribute__((always_inline));
static inline void ili9327SendDataR(uint8_t dataR)
{
    ILI9327_DHI_Port->BSRR = 0x00FF0000 | dataR;    // If port bits 7..0 are used
    CLR(ILI9327_WR);                                // Strob LSB
    SET(ILI9327_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ILI9327_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ILI9327_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ILI9327_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ILI9327_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}


static inline void ili9327SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void ili9327SelectReg(uint8_t reg)
{
    CLR(ILI9327_RS);
    ili9327SendDataR(reg);
    SET(ILI9327_RS);
}

static inline void ili9327InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(ILI9327_CS);

    // Initial Sequence
    ili9327SelectReg(0xE9);
    ili9327SendDataR(0x20);

    ili9327SelectReg(0x11);
    _delay_ms(100);

    ili9327SelectReg(0xD1);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x71);
    ili9327SendDataR(0x19);

    ili9327SelectReg(0xD0);
    ili9327SendDataR(0x07);
    ili9327SendDataR(0x01);
    ili9327SendDataR(0x08);

    ili9327SelectReg(0x36);
    ili9327SendDataR(0x08);

    ili9327SelectReg(0x3A);
    ili9327SendDataR(0x05);

    ili9327SelectReg(0xC1);
    ili9327SendDataR(0x10);
    ili9327SendDataR(0x10);
    ili9327SendDataR(0x02);
    ili9327SendDataR(0x02);

    ili9327SelectReg(0xC0);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x35);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x01);
    ili9327SendDataR(0x02);

    ili9327SelectReg(0xC5);
    ili9327SendDataR(0x04);

    ili9327SelectReg(0xD2);
    ili9327SendDataR(0x01);
    ili9327SendDataR(0x44);

    ili9327SelectReg(0xC8);
    ili9327SendDataR(0x04);
    ili9327SendDataR(0x67);
    ili9327SendDataR(0x35);
    ili9327SendDataR(0x04);
    ili9327SendDataR(0x08);
    ili9327SendDataR(0x06);
    ili9327SendDataR(0x24);
    ili9327SendDataR(0x01);
    ili9327SendDataR(0x37);
    ili9327SendDataR(0x40);
    ili9327SendDataR(0x03);
    ili9327SendDataR(0x10);
    ili9327SendDataR(0x08);
    ili9327SendDataR(0x80);
    ili9327SendDataR(0x00);

    ili9327SelectReg(0x2A);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0xEF);

    ili9327SelectReg(0x2B);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x00);
    ili9327SendDataR(0x01);
    ili9327SendDataR(0x8F);

    ili9327SelectReg(0x29);

    SET(ILI9327_CS);
}

static inline void ili9327SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void ili9327SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

    ili9327SelectReg(0x2A);
    ili9327SendData(y);
    ili9327SendData(y + h - 1);
    ili9327SelectReg(0x2B);
    ili9327SendData(x);
    ili9327SendData(x + w - 1);
}

void ili9327Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc400x240Init(*driver);

    SET(ILI9327_LED);
    SET(ILI9327_RD);
    SET(ILI9327_WR);
    SET(ILI9327_RS);
    SET(ILI9327_CS);

    CLR(ILI9327_RST);
    _delay_ms(1);
    SET(ILI9327_RST);

    ili9327InitSeq();
}

void ili9327Clear(void)
{
    ili9327DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void ili9327BusIRQ(void)
{
    bus_requested = 1;
}

void ili9327Sleep(void)
{
    CLR(ILI9327_CS);

    ili9327SelectReg(0x28);    // Display OFF
    _delay_ms(100);
    ili9327SelectReg(0x10);

    SET(ILI9327_CS);
}

void ili9327Wakeup(void)
{
    CLR(ILI9327_CS);

    ili9327SelectReg(0x11);    // Display OFF
    _delay_ms(100);
    ili9327SelectReg(0x29);

    SET(ILI9327_CS);
}

void ili9327DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(ILI9327_CS);

    ili9327SetWindow(x, y, 1, 1);

    ili9327SelectReg(0x2C);
    ili9327SendData(color);

    SET(ILI9327_CS);
}

void ili9327DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(ILI9327_CS);

    ili9327SetWindow(x, y, w, h);

    ili9327SelectReg(0x2C);
    for (uint32_t i = 0; i < w * h; i++)
        ili9327SendData(color);

    SET(ILI9327_CS);
}

void ili9327DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;
    uint8_t mult = glcd.font.mult;

    CLR(ILI9327_CS);

    ili9327SetWindow(x0, y0, mult * w, mult * h);

    ili9327SelectReg(0x2C);

    glcdSendImage(img, ili9327SendData);

    SET(ILI9327_CS);
}

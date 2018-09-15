#include "spfd5408.h"

#include "../dispcanvas.h"

#include "../../pins.h"
#include "../../functions.h"

#define SPFD5408_WIDTH           240
#define SPFD5408_HEIGHT          320
#define SPFD5408_PIXELS          (SPFD5408_WIDTH * SPFD5408_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = spfd5408Clear,
    .drawPixel = spfd5408DrawPixel,
    .drawRectangle = spfd5408DrawRectangle,
    .drawImage = spfd5408DrawImage,
};

static inline void spfd5408SendData(uint16_t data) __attribute__((always_inline));
static inline void spfd5408SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    SPFD5408_DHI_Port->BSRR = 0x00FF0000 | dataH;   // If port bits 7..0 are used
    CLR(SPFD5408_WR);                               // Strob MSB
    SET(SPFD5408_WR);
    SPFD5408_DHI_Port->BSRR = 0x00FF0000 | dataL;   // If port bits 7..0 are used
    CLR(SPFD5408_WR);                               // Strob LSB
    SET(SPFD5408_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        SPFD5408_DHI_Port->BSRR = 0x000000FF;       // Set 1 on all data lines
        SPFD5408_DHI_Port->CRL = 0x88888888;        // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = SPFD5408_DHI_Port->IDR & 0x00FF; // Read 8-bit bus
        SPFD5408_DHI_Port->CRL = 0x33333333;        // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void spfd5408SelectReg(uint16_t reg) __attribute__((always_inline));
static inline void spfd5408SelectReg(uint16_t reg)
{
    CLR(SPFD5408_RS);
    spfd5408SendData(reg);
    SET(SPFD5408_RS);
}

static void spfd5408WriteReg(uint16_t reg, uint16_t data)
{
    spfd5408SelectReg(reg);
    spfd5408SendData(data);
}

static inline void spfd5408InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(SPFD5408_CS);
    // Initial Sequence
    spfd5408WriteReg(0x0000, 0x0000);
    spfd5408WriteReg(0x0001, 0x0100);
    spfd5408WriteReg(0x0002, 0x0700);
    spfd5408WriteReg(0x0003, 0x1020);
    spfd5408WriteReg(0x0004, 0x0000);
    spfd5408WriteReg(0x0008, 0x0207);
    spfd5408WriteReg(0x0009, 0x0000);
    spfd5408WriteReg(0x000A, 0x0000);
    spfd5408WriteReg(0x000C, 0x0000);
    spfd5408WriteReg(0x000D, 0x0000);
    spfd5408WriteReg(0x000F, 0x0000);
    spfd5408WriteReg(0x0007, 0x0101);

    // Power On Sequence
    spfd5408WriteReg(0x0010, 0x14B0);
    _delay_ms(20);
    spfd5408WriteReg(0x0011, 0x0007);
    _delay_ms(20);
    spfd5408WriteReg(0x0017, 0x0001);
    _delay_ms(20);
    spfd5408WriteReg(0x0012, 0x01B8);
    _delay_ms(20);
    spfd5408WriteReg(0x0013, 0x1300);
    _delay_ms(20);
    spfd5408WriteReg(0x0029, 0x000F);

    // Adjust the Gamma Curve
    spfd5408WriteReg(0x0030, 0x0102);
    spfd5408WriteReg(0x0031, 0x0C18);
    spfd5408WriteReg(0x0032, 0x0F1C);
    spfd5408WriteReg(0x0033, 0x3617);
    spfd5408WriteReg(0x0034, 0x350B);
    spfd5408WriteReg(0x0035, 0x1004);
    spfd5408WriteReg(0x0036, 0x0D06);
    spfd5408WriteReg(0x0037, 0x0318);
    spfd5408WriteReg(0x0038, 0x0604);
    spfd5408WriteReg(0x0039, 0x0005);
    spfd5408WriteReg(0x003A, 0x0A05);
    spfd5408WriteReg(0x003B, 0x0A00);
    spfd5408WriteReg(0x003C, 0x020F);
    spfd5408WriteReg(0x003D, 0x050F);
    spfd5408WriteReg(0x003E, 0x0303);
    spfd5408WriteReg(0x003F, 0x0505);


    // Set GRAM area
    spfd5408WriteReg(0x0020, 0x0000);   // GRAM horisontal address
    spfd5408WriteReg(0x0021, 0x0000);   // GRAM vertical address

    spfd5408WriteReg(0x0050, 0x0000);   // Horizontal GRAM Start Address
    spfd5408WriteReg(0x0051, 0x00EF);   // Horizontal GRAM End Address
    spfd5408WriteReg(0x0052, 0x0000);   // Vertical GRAM Start Address
    spfd5408WriteReg(0x0053, 0x013F);   // Vertical GRAM End Address

    spfd5408WriteReg(0x0060, 0x2700);   // Gate scan line
    spfd5408WriteReg(0x0061, 0x0001);   // NDV, VLE, REV
    spfd5408WriteReg(0x006A, 0x0000);   // Set scrolling line

    // Partial Display Control
    spfd5408WriteReg(0x0080, 0x0000);
    spfd5408WriteReg(0x0081, 0x0000);
    spfd5408WriteReg(0x0082, 0x0000);
    spfd5408WriteReg(0x0083, 0x0000);
    spfd5408WriteReg(0x0084, 0x0000);
    spfd5408WriteReg(0x0085, 0x0000);

    // Panel Control
    spfd5408WriteReg(0x0090, 0x0018);
    spfd5408WriteReg(0x0092, 0x0000);
    spfd5408WriteReg(0x0093, 0x0103);
    spfd5408WriteReg(0x0095, 0x0110);
    spfd5408WriteReg(0x0097, 0x0000);
    spfd5408WriteReg(0x0098, 0x0000);

    spfd5408WriteReg(0x00F0, 0x5408);
    spfd5408WriteReg(0x00F2, 0x00DF);
    spfd5408WriteReg(0x00F3, 0x0006);
    spfd5408WriteReg(0x00F4, 0x001F);
    spfd5408WriteReg(0x00F7, 0x0008);
    spfd5408WriteReg(0x00F0, 0x0000);

    spfd5408WriteReg(0x0007, 0x0173);   // 262K color and display ON
    _delay_ms(150);


    SET(SPFD5408_CS);
}

static inline void spfd5408SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void spfd5408SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    spfd5408WriteReg(0x0050, SPFD5408_WIDTH - y - h);
    spfd5408WriteReg(0x0051, SPFD5408_WIDTH - y - 1);
    spfd5408WriteReg(0x0052, x);
    spfd5408WriteReg(0x0053, x + w - 1);

    // Set cursor
    spfd5408WriteReg(0x0020, SPFD5408_WIDTH - y - 1);
    spfd5408WriteReg(0x0021, x);
}

void spfd5408Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc320x240Init(*driver);

    SET(SPFD5408_LED);
    SET(SPFD5408_RD);
    SET(SPFD5408_WR);
    SET(SPFD5408_RS);
    SET(SPFD5408_CS);

    CLR(SPFD5408_RST);
    _delay_ms(1);
    SET(SPFD5408_RST);

    spfd5408InitSeq();
}

void spfd5408Clear(void)
{
    spfd5408DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void spfd5408BusIRQ(void)
{
    bus_requested = 1;
}

void spfd5408Sleep(void)
{
    CLR(SPFD5408_CS);

    spfd5408WriteReg(0x0007, 0x0000);   // Display OFF
    // Power Off Sequence
    spfd5408WriteReg(0x0010, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0011, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0017, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0012, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0013, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0010, 0x0002);   // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(SPFD5408_CS);
}

void spfd5408Wakeup(void)
{
    CLR(SPFD5408_CS);

    // Power On Sequence
    spfd5408WriteReg(0x0010, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0011, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0017, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0012, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0013, 0x0000);
    _delay_ms(20);
    spfd5408WriteReg(0x0010, 0x14B0);
    _delay_ms(20);
    spfd5408WriteReg(0x0011, 0x0007);
    _delay_ms(20);
    spfd5408WriteReg(0x0017, 0x0001);
    _delay_ms(20);
    spfd5408WriteReg(0x0012, 0x01B8);
    _delay_ms(20);
    spfd5408WriteReg(0x0013, 0x1300);
    _delay_ms(20);
    spfd5408WriteReg(0x0029, 0x000F);

    spfd5408WriteReg(0x0007, 0x0173);   // 262K color and display ON

    SET(SPFD5408_CS);
}

void spfd5408DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(SPFD5408_CS);

    spfd5408SetWindow(x, y, 1, 1);

    spfd5408SelectReg(0x0022);
    spfd5408SendData(color);

    SET(SPFD5408_CS);
}

void spfd5408DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(SPFD5408_CS);

    spfd5408SetWindow(x, y, w, h);

    spfd5408SelectReg(0x0022);
    for (uint32_t i = 0; i < w * h; i++)
        spfd5408SendData(color);

    SET(SPFD5408_CS);
}

void spfd5408DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(SPFD5408_CS);

    spfd5408SetWindow(x0, y0, w, h);

    spfd5408SelectReg(0x0022);

    DISPDRV_SEND_IMAGE(img, spfd5408SendData);

    SET(SPFD5408_CS);
}

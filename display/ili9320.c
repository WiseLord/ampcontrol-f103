#include "ili9320.h"

#include "../pins.h"
#include "../functions.h"

#include "gc320x240.h"

#define ILI9320_WIDTH           240
#define ILI9320_HEIGHT          320
#define ILI9320_PIXELS          (ILI9320_WIDTH * ILI9320_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = ili9320Clear,
    .drawPixel = ili9320DrawPixel,
    .drawRectangle = ili9320DrawRectangle,
    .drawImage = ili9320DrawImage,
};

static inline void ili9320SendData(uint16_t data) __attribute__((always_inline));
static inline void ili9320SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ILI9320_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(ILI9320_WR);                                // Strob MSB
    SET(ILI9320_WR);
    ILI9320_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(ILI9320_WR);                                // Strob LSB
    SET(ILI9320_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ILI9320_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ILI9320_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ILI9320_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ILI9320_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void ili9320SelectReg(uint16_t reg) __attribute__((always_inline));
static inline void ili9320SelectReg(uint16_t reg)
{
    CLR(ILI9320_RS);
    ili9320SendData(reg);
    SET(ILI9320_RS);
}

static void ili9320WriteReg(uint16_t reg, uint16_t data)
{
    ili9320SelectReg(reg);
    ili9320SendData(data);
}

static inline void ili9320InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(ILI9320_CS);

    // Initial Sequence
    ili9320WriteReg(0x00E5, 0x8000);    // Set the Vcore voltage and this setting is must
    ili9320WriteReg(0x0000, 0x0001);    // Start internal OSC
    ili9320WriteReg(0x0001, 0x0000);    // Set SS and SM bit
    ili9320WriteReg(0x0002, 0x0700);    // Set 1 line inversion
    ili9320WriteReg(0x0003, 0x1020);    // Set GRAM write direction and BGR = 1
    ili9320WriteReg(0x0004, 0x0000);    // Resize register

    ili9320WriteReg(0x0008, 0x0202);    // Set the back porch and front porch
    ili9320WriteReg(0x0009, 0x0000);    // Set non-display area refresh cycle ICS[3:0]
    ili9320WriteReg(0x000A, 0x0000);    // FMARK function
    ili9320WriteReg(0x000C, 0x0000);    // RGB interface setting
    ili9320WriteReg(0x000D, 0x0000);    // Frame marker position
    ili9320WriteReg(0x000F, 0x0000);    // RGB interface polarity

    // Power On Sequence
    ili9320WriteReg(0x0010, 0x17B0);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0037);    // DC1[2:0], DC0[2:0], VC[2:0]
    ili9320WriteReg(0x0012, 0x013A);    // VREG1OUT voltage
    ili9320WriteReg(0x0013, 0x1600);    // VDV[4:0] for VCOM amplitude
    ili9320WriteReg(0x0029, 0x000C);    // VCM[4:0] for VCOMH

    // Adjust the Gamma Curve
    ili9320WriteReg(0x0030, 0x0007);
    ili9320WriteReg(0x0031, 0x0700);
    ili9320WriteReg(0x0032, 0x0707);
    ili9320WriteReg(0x0035, 0x0007);
    ili9320WriteReg(0x0036, 0x000F);
    ili9320WriteReg(0x0037, 0x0700);
    ili9320WriteReg(0x0038, 0x0707);
    ili9320WriteReg(0x0039, 0x0007);
    ili9320WriteReg(0x003C, 0x0700);
    ili9320WriteReg(0x003D, 0x0F00);

    // Set GRAM area
    ili9320WriteReg(0x0020, 0x0000);    // GRAM horisontal address
    ili9320WriteReg(0x0021, 0x0000);    // GRAM vertical address

    ili9320WriteReg(0x0050, 0x0000);    // Horizontal GRAM Start Address
    ili9320WriteReg(0x0051, 0x00EF);    // Horizontal GRAM End Address
    ili9320WriteReg(0x0052, 0x0000);    // Vertical GRAM Start Address
    ili9320WriteReg(0x0053, 0x013F);    // Vertical GRAM End Address

    ili9320WriteReg(0x0060, 0x2700);    // Gate scan line
    ili9320WriteReg(0x0061, 0x0003);    // NDV, VLE, REV
    ili9320WriteReg(0x006A, 0x0000);    // Set scrolling line

    // Partial Display Control
    ili9320WriteReg(0x0080, 0x0000);
    ili9320WriteReg(0x0081, 0x0000);
    ili9320WriteReg(0x0082, 0x0000);
    ili9320WriteReg(0x0083, 0x0000);
    ili9320WriteReg(0x0084, 0x0000);
    ili9320WriteReg(0x0085, 0x0000);

    // Panel Control
    ili9320WriteReg(0x0090, 0x0010);
    ili9320WriteReg(0x0092, 0x0000);
    ili9320WriteReg(0x0093, 0x0003);
    ili9320WriteReg(0x0095, 0x0110);
    ili9320WriteReg(0x0097, 0x0000);
    ili9320WriteReg(0x0098, 0x0000);

    ili9320WriteReg(0x0007, 0x0173);    // 262K color and display ON

    SET(ILI9320_CS);
}

static inline void ili9320SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void ili9320SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9320WriteReg(0x0050, ILI9320_WIDTH - y - h);
    ili9320WriteReg(0x0051, ILI9320_WIDTH - y - 1);
    ili9320WriteReg(0x0052, x);
    ili9320WriteReg(0x0053, x + w - 1);

    // Set cursor
    ili9320WriteReg(0x0020, ILI9320_WIDTH - y - 1);
    ili9320WriteReg(0x0021, x);
}

void ili9320Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc320x240Init(*driver);

    SET(ILI9320_LED);
    SET(ILI9320_RD);
    SET(ILI9320_WR);
    SET(ILI9320_RS);
    SET(ILI9320_CS);

    CLR(ILI9320_RST);
    _delay_ms(1);
    SET(ILI9320_RST);

    ili9320InitSeq();
}

void ili9320Clear(void)
{
    ili9320DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void ili9320BusIRQ(void)
{
    bus_requested = 1;
}

void ili9320Sleep(void)
{
    CLR(ILI9320_CS);

    ili9320WriteReg(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    ili9320WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    ili9320WriteReg(0x0012, 0x0000);    // VREG1OUT voltage
    ili9320WriteReg(0x0013, 0x0000);    // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    ili9320WriteReg(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(ILI9320_CS);
}

void ili9320Wakeup(void)
{
    CLR(ILI9320_CS);

    // Power On Sequence
    ili9320WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    ili9320WriteReg(0x0012, 0x0000);    // VREG1OUT voltage
    ili9320WriteReg(0x0013, 0x0000);    // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    ili9320WriteReg(0x0010, 0x17B0);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    ili9320WriteReg(0x0011, 0x0037);    // DC1[2:0], DC0[2:0], VC[2:0]
    _delay_ms(50);
    ili9320WriteReg(0x0012, 0x013A);    // VREG1OUT voltage
    _delay_ms(50);
    ili9320WriteReg(0x0013, 0x1600);    // VDV[4:0] for VCOM amplitude
    ili9320WriteReg(0x0029, 0x000C);    // VCM[4:0] for VCOMH

    ili9320WriteReg(0x0007, 0x0173);    // 262K color and display ON

    SET(ILI9320_CS);
}

void ili9320DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(ILI9320_CS);

    ili9320SetWindow(x, y, 1, 1);

    ili9320SelectReg(0x0022);
    ili9320SendData(color);

    SET(ILI9320_CS);
}

void ili9320DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(ILI9320_CS);

    ili9320SetWindow(x, y, w, h);

    ili9320SelectReg(0x0022);
    for (uint32_t i = 0; i < w * h; i++)
        ili9320SendData(color);

    SET(ILI9320_CS);
}

void ili9320DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;
    uint16_t color = glcd.font.color;
    uint16_t bgColor = glcd.canvas->color;
    uint8_t mult = glcd.font.mult;

    CLR(ILI9320_CS);

    ili9320SetWindow(x0, y0, mult * w, mult * h * 8);

    ili9320SelectReg(0x0022);
    for (uint16_t i = 0; i < w; i++) {
        for (uint8_t mx = 0; mx < mult; mx++) {
            for (uint16_t j = 0; j < h; j++) {
                uint8_t data = img->data[w * j + i];
                for (uint8_t bit = 0; bit < 8; bit++) {
                    for (uint8_t my = 0; my < mult; my++)
                        ili9320SendData(data & 0x01 ? color : bgColor);
                    data >>= 1;
                }
            }
        }
    }

    SET(ILI9320_CS);
}

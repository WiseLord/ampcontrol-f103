#include "s6d0139.h"

#include "../../pins.h"
#include "../../functions.h"

#define S6D0139_WIDTH           240
#define S6D0139_HEIGHT          320
#define S6D0139_PIXELS          (S6D0139_WIDTH * S6D0139_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = s6d0139Clear,
    .drawPixel = s6d0139DrawPixel,
    .drawRectangle = s6d0139DrawRectangle,
    .drawImage = s6d0139DrawImage,
};

static inline void s6d0139SendData(uint16_t data) __attribute__((always_inline));
static inline void s6d0139SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    S6D0139_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(S6D0139_WR);                                // Strob MSB
    SET(S6D0139_WR);
    S6D0139_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(S6D0139_WR);                                // Strob LSB
    SET(S6D0139_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        S6D0139_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        S6D0139_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = S6D0139_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        S6D0139_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void s6d0139SelectReg(uint16_t reg) __attribute__((always_inline));
static inline void s6d0139SelectReg(uint16_t reg)
{
    CLR(S6D0139_RS);
    s6d0139SendData(reg);
    SET(S6D0139_RS);
}

static void s6d0139WriteReg(uint16_t reg, uint16_t data)
{
    s6d0139SelectReg(reg);
    s6d0139SendData(data);
}

static inline void s6d0139InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(S6D0139_CS);

    s6d0139WriteReg(0x0000, 0x0001);    // Start Oscillation
    _delay_ms(10);
    s6d0139WriteReg(0x0007, 0x0000);    // Display control1
    s6d0139WriteReg(0x0013, 0x0000);    // Power control4 setting
    s6d0139WriteReg(0x0011, 0x2604);    // Power control2 setting
    s6d0139WriteReg(0x0014, 0x0015);    // Power control5 setting
    s6d0139WriteReg(0x0010, 0x3C00);    // Power control1 setting
    s6d0139WriteReg(0x0013, 0x0040);    // Power control4 setting
    _delay_ms(10);
    s6d0139WriteReg(0x0013, 0x0060);    // Power control4 setting
    _delay_ms(50);
    s6d0139WriteReg(0x0013, 0x0070);    // Power control4 setting
    _delay_ms(40);

    s6d0139WriteReg(0x0001, 0x0127);    // Driver output setting (240x320 mode, GS=0, SS=1)
    s6d0139WriteReg(0x0002, 0x0700);    // LCD driving waveform setting
    s6d0139WriteReg(0x0003, 0x1020);    // Entry mode setting (TRI=0, DFM=0, BGR=1, ID1:ID0 =11)
    s6d0139WriteReg(0x0007, 0x0000);    // Display control1
    s6d0139WriteReg(0x0008, 0x0202);    // Display control2
    s6d0139WriteReg(0x000B, 0x0200);    // Frame cycle setting
    s6d0139WriteReg(0x000C, 0x0000);    // External display interface setting
    s6d0139WriteReg(0x0015, 0x0000);

    //gamma setting
    s6d0139WriteReg(0x0030, 0x0000);    // Gamma control1
    s6d0139WriteReg(0x0031, 0x0606);    // Gamma control2
    s6d0139WriteReg(0x0032, 0x0006);    // Gamma control3
    s6d0139WriteReg(0x0033, 0x0403);    // Gamma control4
    s6d0139WriteReg(0x0034, 0x0107);    // Gamma control5
    s6d0139WriteReg(0x0035, 0x0101);    // Gamma control6
    s6d0139WriteReg(0x0036, 0x0707);    // Gamma control7
    s6d0139WriteReg(0x0037, 0x0304);    // Gamma control8
    s6d0139WriteReg(0x0038, 0x0A00);    // Gamma control9
    s6d0139WriteReg(0x0039, 0x0706);    // Gamma control10

    s6d0139WriteReg(0x0040, 0x0000);    // Gate scan position (start G1 scan)
    s6d0139WriteReg(0x0041, 0x0000);    // Vertical scroll setting
    s6d0139WriteReg(0x0042, 0x013F);    // Screen end position (320-1)
    s6d0139WriteReg(0x0043, 0x0000);    // Screen start position (0)
    s6d0139WriteReg(0x0044, 0x0000);    // 2th Screen end position
    s6d0139WriteReg(0x0045, 0x0000);    // 2th Screen start position
    s6d0139WriteReg(0x0046, 0xEF00);    // window addr set for x0,x1 (0, 239)
    s6d0139WriteReg(0x0047, 0x013F);    // window addr set for y0    (319)
    s6d0139WriteReg(0x0048, 0x0000);    // window addr set for y1    (0)
    s6d0139WriteReg(0x0007, 0x0011);    // Display control1
    _delay_ms(40);
    s6d0139WriteReg(0x0007, 0x0017);    // Display control1

    SET(S6D0139_CS);
}

static inline void s6d0139SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void s6d0139SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    s6d0139WriteReg(0x0046, ((S6D0139_WIDTH - y - 1) << 8) + (S6D0139_WIDTH - y - h));
    s6d0139WriteReg(0x0048, x);
    s6d0139WriteReg(0x0047, x + w - 1);

    s6d0139WriteReg(0x0020, S6D0139_WIDTH - y - 1);
    s6d0139WriteReg(0x0021, x);
}

void s6d0139Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc320x240Init(*driver);

    SET(S6D0139_LED);
    SET(S6D0139_RD);
    SET(S6D0139_WR);
    SET(S6D0139_RS);
    SET(S6D0139_CS);

    CLR(S6D0139_RST);
    _delay_ms(1);
    SET(S6D0139_RST);

    s6d0139InitSeq();
}

void s6d0139Clear(void)
{
    s6d0139DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void s6d0139BusIRQ(void)
{
    bus_requested = 1;
}

void s6d0139Sleep(void)
{
    CLR(S6D0139_CS);

    s6d0139WriteReg(0x0007, 0x0000);    // Display OFF
    // Power Off Sequence
    s6d0139WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    s6d0139WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    s6d0139WriteReg(0x0013, 0x0000);    // VREG1OUT voltage

    s6d0139WriteReg(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    s6d0139WriteReg(0x0010, 0x0002);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(S6D0139_CS);
}

void s6d0139Wakeup(void)
{
    CLR(S6D0139_CS);

    // Power On Sequence
    s6d0139WriteReg(0x0010, 0x0000);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    s6d0139WriteReg(0x0011, 0x0000);    // DC1[2:0], DC0[2:0], VC[2:0]
    s6d0139WriteReg(0x0013, 0x0000);    // VREG1OUT voltage
    s6d0139WriteReg(0x0014, 0x0000);    // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    s6d0139WriteReg(0x0007, 0x0000);    // Display control1
    s6d0139WriteReg(0x0013, 0x0000);    // Power control4 setting
    s6d0139WriteReg(0x0011, 0x2604);    // Power control2 setting
    s6d0139WriteReg(0x0014, 0x0015);    // Power control5 setting
    s6d0139WriteReg(0x0010, 0x3C00);    // Power control1 setting
    s6d0139WriteReg(0x0013, 0x0040);    // Power control4 setting
    _delay_ms(10);
    s6d0139WriteReg(0x0013, 0x0060);    // Power control4 setting
    _delay_ms(50);
    s6d0139WriteReg(0x0013, 0x0070);    // Power control4 setting
    _delay_ms(40);
    s6d0139WriteReg(0x0007, 0x0017);    // 262K color and display ON

    SET(S6D0139_CS);
}

void s6d0139DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(S6D0139_CS);

    s6d0139SetWindow(x, y, 1, 1);

    s6d0139SelectReg(0x0022);
    s6d0139SendData(color);

    SET(S6D0139_CS);
}

void s6d0139DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(S6D0139_CS);

    s6d0139SetWindow(x, y, w, h);

    s6d0139SelectReg(0x0022);
    for (uint32_t i = 0; i < w * h; i++)
        s6d0139SendData(color);

    SET(S6D0139_CS);
}

void s6d0139DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;
    uint16_t color = glcd.font.color;
    uint16_t bgColor = glcd.canvas->color;
    uint8_t mult = glcd.font.mult;

    CLR(S6D0139_CS);

    s6d0139SetWindow(x0, y0, mult * w, mult * h);

    s6d0139SelectReg(0x0022);
    for (uint16_t i = 0; i < w; i++) {
        for (uint8_t mx = 0; mx < mult; mx++) {
            for (uint16_t j = 0; j < h + 7 / 8; j++) {
                uint8_t data = img->data[w * j + i];
                for (uint8_t bit = 0; bit < 8; bit++) {
                    if (8 * j + bit < h) {
                        for (uint8_t my = 0; my < mult; my++)
                            s6d0139SendData(data & 0x01 ? color : bgColor);
                        data >>= 1;
                    }
                }
            }
        }
    }

    SET(S6D0139_CS);
}

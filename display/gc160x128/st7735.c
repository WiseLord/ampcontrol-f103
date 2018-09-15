#include "st7735.h"

#include "../../pins.h"
#include "../../functions.h"

#define ST7735_WIDTH           128
#define ST7735_HEIGHT          160
#define ST7735_PIXELS          (ST7735_WIDTH * ST7735_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = st7735Clear,
    .drawPixel = st7735DrawPixel,
    .drawRectangle = st7735DrawRectangle,
    .drawImage = st7735DrawImage,
};

static inline void st7735SendData(uint16_t data) __attribute__((always_inline));
static inline void st7735SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ST7735_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(ST7735_WR);                                // Strob MSB
    SET(ST7735_WR);
    ST7735_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(ST7735_WR);                                // Strob LSB
    SET(ST7735_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ST7735_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ST7735_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ST7735_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ST7735_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void st7735SendDataR(uint8_t dataR) __attribute__((always_inline));
static inline void st7735SendDataR(uint8_t dataR)
{
    ST7735_DHI_Port->BSRR = 0x00FF0000 | dataR;    // If port bits 7..0 are used
    CLR(ST7735_WR);                                // Strob LSB
    SET(ST7735_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ST7735_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ST7735_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ST7735_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ST7735_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}


static inline void st7735SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void st7735SelectReg(uint8_t reg)
{
    CLR(ST7735_RS);
    st7735SendDataR(reg);
    SET(ST7735_RS);
}

static inline void st7735InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(ST7735_CS);

    // Initial Sequence
    //************* Start Initial Sequence **********//
        st7735SelectReg(0x11); //Exit Sleep
        _delay_ms(20);

        st7735SelectReg(0x21); //Display Inversion On

        st7735SelectReg(0x26); //Set Default Gamma
        st7735SendDataR(0x04);

        st7735SelectReg(0xB1);
        st7735SendDataR(0x0C);
        st7735SendDataR(0x14);

        st7735SelectReg(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
        st7735SendDataR(0x0C);
        st7735SendDataR(0x05);

        st7735SelectReg(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
        st7735SendDataR(0x02);

        st7735SelectReg(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
        st7735SendDataR(0x29);
        st7735SendDataR(0x43);

        st7735SelectReg(0xC7);
        st7735SendDataR(0x40);

        st7735SelectReg(0x3a); //Set Color Format
        st7735SendDataR(0x05);

        st7735SelectReg(0x2A); //Set Column Address
        st7735SendDataR(0x00);
        st7735SendDataR(0x00);
        st7735SendDataR(0x00);
        st7735SendDataR(0x7F);

        st7735SelectReg(0x2B); //Set Page Address
        st7735SendDataR(0x00);
        st7735SendDataR(0x00);
        st7735SendDataR(0x00);
        st7735SendDataR(0x9F);

        st7735SelectReg(0x36); //Set Scanning Direction
        st7735SendDataR(0x80);

        st7735SelectReg(0xB7); //Set Source Output Direction
        st7735SendDataR(0x00);

        st7735SelectReg(0xE0);
        st7735SendDataR(0x36);//p1
        st7735SendDataR(0x29);//p2
        st7735SendDataR(0x12);//p3
        st7735SendDataR(0x22);//p4
        st7735SendDataR(0x1C);//p5
        st7735SendDataR(0x15);//p6
        st7735SendDataR(0x42);//p7
        st7735SendDataR(0xB7);//p8
        st7735SendDataR(0x2F);//p9
        st7735SendDataR(0x13);//p10
        st7735SendDataR(0x12);//p11
        st7735SendDataR(0x0A);//p12
        st7735SendDataR(0x11);//p13
        st7735SendDataR(0x0B);//p14
        st7735SendDataR(0x06);//p15

        st7735SelectReg(0xE1);
        st7735SendDataR(0x09);//p1
        st7735SendDataR(0x16);//p2
        st7735SendDataR(0x2D);//p3
        st7735SendDataR(0x0D);//p4
        st7735SendDataR(0x13);//p5
        st7735SendDataR(0x15);//p6
        st7735SendDataR(0x40);//p7
        st7735SendDataR(0x48);//p8
        st7735SendDataR(0x53);//p9
        st7735SendDataR(0x0C);//p10
        st7735SendDataR(0x1D);//p11
        st7735SendDataR(0x25);//p12
        st7735SendDataR(0x2E);//p13
        st7735SendDataR(0x34);//p14
        st7735SendDataR(0x39);//p15

        st7735SelectReg(0x29); // Display On

    SET(ST7735_CS);
}

static inline void st7735SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void st7735SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

    st7735SelectReg(0x2A);
    st7735SendData(y);
    st7735SendData(y + h - 1);
    st7735SelectReg(0x2B);
    st7735SendData(x);
    st7735SendData(x + w - 1);
}

void st7735Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc160x128Init(*driver);

    SET(ST7735_LED);
    SET(ST7735_RD);
    SET(ST7735_WR);
    SET(ST7735_RS);
    SET(ST7735_CS);

    CLR(ST7735_RST);
    _delay_ms(1);
    SET(ST7735_RST);

    st7735InitSeq();
}

void st7735Clear(void)
{
    st7735DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void st7735BusIRQ(void)
{
    bus_requested = 1;
}

void st7735Sleep(void)
{
    CLR(ST7735_CS);

    st7735SelectReg(0x28);    // Display OFF
    _delay_ms(100);
    st7735SelectReg(0x10);

    SET(ST7735_CS);
}

void st7735Wakeup(void)
{
    CLR(ST7735_CS);

    st7735SelectReg(0x11);    // Display OFF
    _delay_ms(100);
    st7735SelectReg(0x29);

    SET(ST7735_CS);
}

void st7735DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(ST7735_CS);

    st7735SetWindow(x, y, 1, 1);

    st7735SelectReg(0x2C);
    st7735SendData(color);

    SET(ST7735_CS);
}

void st7735DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(ST7735_CS);

    st7735SetWindow(x, y, w, h);

    st7735SelectReg(0x2C);
    for (uint32_t i = 0; i < w * h; i++)
        st7735SendData(color);

    SET(ST7735_CS);
}

void st7735DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(ST7735_CS);

    st7735SetWindow(x0, y0, w, h);

    st7735SelectReg(0x2C);
    
    DISPDRV_SEND_IMAGE(img, st7735SendData);

    SET(ST7735_CS);
}

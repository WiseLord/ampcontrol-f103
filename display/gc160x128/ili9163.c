#include "ili9163.h"

#include "../dispcanvas.h"

#include "../../pins.h"
#include "../../functions.h"

#define ILI9163_WIDTH           128
#define ILI9163_HEIGHT          160
#define ILI9163_PIXELS          (ILI9163_WIDTH * ILI9163_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = ili9163Clear,
    .drawPixel = ili9163DrawPixel,
    .drawRectangle = ili9163DrawRectangle,
    .drawImage = ili9163DrawImage,
};

static inline void ili9163SendData(uint16_t data) __attribute__((always_inline));
static inline void ili9163SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ILI9163_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(ILI9163_WR);                                // Strob MSB
    SET(ILI9163_WR);
    ILI9163_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(ILI9163_WR);                                // Strob LSB
    SET(ILI9163_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ILI9163_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ILI9163_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ILI9163_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ILI9163_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void ili9163SendDataR(uint8_t dataR) __attribute__((always_inline));
static inline void ili9163SendDataR(uint8_t dataR)
{
    ILI9163_DHI_Port->BSRR = 0x00FF0000 | dataR;    // If port bits 7..0 are used
    CLR(ILI9163_WR);                                // Strob LSB
    SET(ILI9163_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ILI9163_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ILI9163_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ILI9163_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ILI9163_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}


static inline void ili9163SelectReg(uint8_t reg) __attribute__((always_inline));
static inline void ili9163SelectReg(uint8_t reg)
{
    CLR(ILI9163_RS);
    ili9163SendDataR(reg);
    SET(ILI9163_RS);
}

static inline void ili9163InitSeq(void)
{
    // Wait for reset
    _delay_ms(50);

    CLR(ILI9163_CS);

    // Initial Sequence
    //************* Start Initial Sequence **********//
    ili9163SelectReg(0x11); //Exit Sleep
    _delay_ms(20);

    ili9163SelectReg(0x26); //Set Default Gamma
    ili9163SendDataR(0x04);

    ili9163SelectReg(0xB1);
    ili9163SendDataR(0x0C);
    ili9163SendDataR(0x14);

    ili9163SelectReg(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    ili9163SendDataR(0x0C);
    ili9163SendDataR(0x05);

    ili9163SelectReg(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    ili9163SendDataR(0x02);

    ili9163SelectReg(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    ili9163SendDataR(0x29);
    ili9163SendDataR(0x43);

    ili9163SelectReg(0xC7);
    ili9163SendDataR(0x40);

    ili9163SelectReg(0x3a); //Set Color Format
    ili9163SendDataR(0x05);

    ili9163SelectReg(0x2A); //Set Column Address
    ili9163SendDataR(0x00);
    ili9163SendDataR(0x00);
    ili9163SendDataR(0x00);
    ili9163SendDataR(0x7F);

    ili9163SelectReg(0x2B); //Set Page Address
    ili9163SendDataR(0x00);
    ili9163SendDataR(0x00);
    ili9163SendDataR(0x00);
    ili9163SendDataR(0x9F);

    ili9163SelectReg(0x36); //Set Scanning Direction
    ili9163SendDataR(0x88);

    ili9163SelectReg(0xB7); //Set Source Output Direction
    ili9163SendDataR(0x00);

    ili9163SelectReg(0xf2); //Enable Gamma bit
    ili9163SendDataR(0x01);

    ili9163SelectReg(0xE0);
    ili9163SendDataR(0x36);//p1
    ili9163SendDataR(0x29);//p2
    ili9163SendDataR(0x12);//p3
    ili9163SendDataR(0x22);//p4
    ili9163SendDataR(0x1C);//p5
    ili9163SendDataR(0x15);//p6
    ili9163SendDataR(0x42);//p7
    ili9163SendDataR(0xB7);//p8
    ili9163SendDataR(0x2F);//p9
    ili9163SendDataR(0x13);//p10
    ili9163SendDataR(0x12);//p11
    ili9163SendDataR(0x0A);//p12
    ili9163SendDataR(0x11);//p13
    ili9163SendDataR(0x0B);//p14
    ili9163SendDataR(0x06);//p15

    ili9163SelectReg(0xE1);
    ili9163SendDataR(0x09);//p1
    ili9163SendDataR(0x16);//p2
    ili9163SendDataR(0x2D);//p3
    ili9163SendDataR(0x0D);//p4
    ili9163SendDataR(0x13);//p5
    ili9163SendDataR(0x15);//p6
    ili9163SendDataR(0x40);//p7
    ili9163SendDataR(0x48);//p8
    ili9163SendDataR(0x53);//p9
    ili9163SendDataR(0x0C);//p10
    ili9163SendDataR(0x1D);//p11
    ili9163SendDataR(0x25);//p12
    ili9163SendDataR(0x2E);//p13
    ili9163SendDataR(0x34);//p14
    ili9163SendDataR(0x39);//p15

    ili9163SelectReg(0x29); // Display On

    SET(ILI9163_CS);
}

static inline void ili9163SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void ili9163SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

    ili9163SelectReg(0x2A);
    ili9163SendData(y);
    ili9163SendData(y + h - 1);
    ili9163SelectReg(0x2B);
    ili9163SendData(x);
    ili9163SendData(x + w - 1);
}

void ili9163Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc160x128Init(*driver);

    SET(ILI9163_LED);
    SET(ILI9163_RD);
    SET(ILI9163_WR);
    SET(ILI9163_RS);
    SET(ILI9163_CS);

    CLR(ILI9163_RST);
    _delay_ms(1);
    SET(ILI9163_RST);

    ili9163InitSeq();
}

void ili9163Clear(void)
{
    ili9163DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void ili9163BusIRQ(void)
{
    bus_requested = 1;
}

void ili9163Sleep(void)
{
    CLR(ILI9163_CS);

    ili9163SelectReg(0x28);    // Display OFF
    _delay_ms(100);
    ili9163SelectReg(0x10);

    SET(ILI9163_CS);
}

void ili9163Wakeup(void)
{
    CLR(ILI9163_CS);

    ili9163SelectReg(0x11);    // Display OFF
    _delay_ms(100);
    ili9163SelectReg(0x29);

    SET(ILI9163_CS);
}

void ili9163DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(ILI9163_CS);

    ili9163SetWindow(x, y, 1, 1);

    ili9163SelectReg(0x2C);
    ili9163SendData(color);

    SET(ILI9163_CS);
}

void ili9163DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(ILI9163_CS);

    ili9163SetWindow(x, y, w, h);

    ili9163SelectReg(0x2C);
    for (uint32_t i = 0; i < w * h; i++)
        ili9163SendData(color);

    SET(ILI9163_CS);
}

void ili9163DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;
   
    CLR(ILI9163_CS);

    ili9163SetWindow(x0, y0, w, h);

    ili9163SelectReg(0x2C);
    
    DISPDRV_SEND_IMAGE(img, ili9163SendData);

    SET(ILI9163_CS);
}

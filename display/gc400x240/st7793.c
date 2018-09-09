#include "st7793.h"

#include "../../pins.h"
#include "../../functions.h"

#define ST7793_WIDTH           240
#define ST7793_HEIGHT          400
#define ST7793_PIXELS          (ST7793_WIDTH * ST7793_HEIGHT)

static uint8_t bus_requested = 0;

static GlcdDriver glcd = {
    .clear = st7793Clear,
    .drawPixel = st7793DrawPixel,
    .drawRectangle = st7793DrawRectangle,
    .drawImage = st7793DrawImage,
};

static inline void st7793SendData(uint16_t data) __attribute__((always_inline));
static inline void st7793SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ST7793_DHI_Port->BSRR = 0x00FF0000 | dataH;    // If port bits 7..0 are used
    CLR(ST7793_WR);                                // Strob MSB
    SET(ST7793_WR);
    ST7793_DHI_Port->BSRR = 0x00FF0000 | dataL;    // If port bits 7..0 are used
    CLR(ST7793_WR);                                // Strob LSB
    SET(ST7793_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ST7793_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        ST7793_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd.bus = ST7793_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        ST7793_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

static inline void st7793SelectReg(uint16_t reg) __attribute__((always_inline));
static inline void st7793SelectReg(uint16_t reg)
{
    CLR(ST7793_RS);
    st7793SendData(reg);
    SET(ST7793_RS);
}

static void st7793WriteReg(uint16_t reg, uint16_t data)
{
    st7793SelectReg(reg);
    st7793SendData(data);
}

static inline void st7793InitSeq(void)
{
    // Wait for reset
    _delay_ms(200);

    CLR(ST7793_CS);

    //-------------Display Control Setting-------------------------------------//

    st7793WriteReg(0x0001, 0x0100);
    st7793WriteReg(0x0003, 0x1020);
    st7793WriteReg(0x0008, 0x0808);
    st7793WriteReg(0x0090, 0x8000);
    st7793WriteReg(0x0400, 0x6200);
    st7793WriteReg(0x0401, 0x0001);

    //-------------End Display Control setting---------------------------------//
    //-------------Power Control Registers Initial ----------------------------//

    st7793WriteReg(0x00ff, 0x0001);
    st7793WriteReg(0x0102, 0x01b0);
    st7793WriteReg(0x0710, 0x0016);
    st7793WriteReg(0x0712, 0x000f);
    st7793WriteReg(0x0752, 0x002f);
    st7793WriteReg(0x0759, 0x0070);

    //--------------End Power Control Registers Initial ------------------------//
    _delay_ms(100);
    //--------------Display Windows 240 X 400-----------------------------------//

    st7793WriteReg(0x0210, 0x0000);
    st7793WriteReg(0x0211, 0x00ef);
    st7793WriteReg(0x0212, 0x0000);
    st7793WriteReg(0x0213, 0x018f);

    //--------------End Display Windows 240 X 400-------------------------------//
    _delay_ms(10);
    //--------------Gamma Cluster Setting---------------------------------------//

    st7793WriteReg(0x0380, 0x0100);
    st7793WriteReg(0x0381, 0x3718);
    st7793WriteReg(0x0382, 0x0802);
    st7793WriteReg(0x0383, 0x060f);
    st7793WriteReg(0x0384, 0x2223);
    st7793WriteReg(0x0385, 0x0000);
    st7793WriteReg(0x0386, 0x5816);
    st7793WriteReg(0x0387, 0x0503);
    st7793WriteReg(0x0388, 0x0510);
    st7793WriteReg(0x0389, 0x2233);

    //---------------End Gamma Setting------------------------------------------//
    //---------------Vcom Setting-----------------------------------------------//

    st7793WriteReg(0x0702, 0x0057);
    st7793WriteReg(0x00ff, 0x0000);

    //---------------End Vcom Setting-------------------------------------------//

    st7793WriteReg(0x0007, 0x0100);
    _delay_ms(200);
    st7793WriteReg(0x0200, 0x0000);
    st7793WriteReg(0x0201, 0x0000);

    SET(ST7793_CS);
}

static inline void st7793SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) __attribute__((always_inline));
static inline void st7793SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    st7793WriteReg(0x0210, ST7793_WIDTH - y - h);
    st7793WriteReg(0x0211, ST7793_WIDTH - y - 1);
    st7793WriteReg(0x0212, x);
    st7793WriteReg(0x0213, x + w - 1);

    // Set cursor
    st7793WriteReg(0x00200, ST7793_WIDTH - y - 1);
    st7793WriteReg(0x00201, x);
}

void st7793Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc400x240Init(*driver);

    SET(ST7793_LED);
    SET(ST7793_RD);
    SET(ST7793_WR);
    SET(ST7793_RS);
    SET(ST7793_CS);

    CLR(ST7793_RST);
    _delay_ms(1);
    SET(ST7793_RST);

    st7793InitSeq();
}

void st7793Clear(void)
{
    st7793DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, LCD_COLOR_BLACK);
}

void st7793BusIRQ(void)
{
    bus_requested = 1;
}

void st7793Sleep(void)
{
    CLR(ST7793_CS);

    st7793WriteReg(0x0007, 0x0000);
    _delay_ms(50);
    st7793WriteReg(0x0102, 0x0180);
    _delay_ms(200);

    SET(ST7793_CS);
}

void st7793Wakeup(void)
{
    CLR(ST7793_CS);

    // Power On Sequence
    _delay_ms(200);
    st7793WriteReg(0x0102, 0x01b0);
    _delay_ms(50);
    st7793WriteReg(0x0007, 0x0100);

    SET(ST7793_CS);
}

void st7793DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(ST7793_CS);

    st7793SetWindow(x, y, 1, 1);

    st7793SelectReg(0x0202);
    st7793SendData(color);

    SET(ST7793_CS);
}

void st7793DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(ST7793_CS);

    st7793SetWindow(x, y, w, h);

    st7793SelectReg(0x0202);
    for (uint32_t i = 0; i < w * h; i++)
        st7793SendData(color);

    SET(ST7793_CS);
}

void st7793DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;
    uint16_t color = glcd.font.color;
    uint16_t bgColor = glcd.canvas->color;
    uint8_t mult = glcd.font.mult;

    CLR(ST7793_CS);

    st7793SetWindow(x0, y0, mult * w, mult * h);

    st7793SelectReg(0x0202);
    for (uint16_t i = 0; i < w; i++) {
        for (uint8_t mx = 0; mx < mult; mx++) {
            for (uint16_t j = 0; j < h + 7 / 8; j++) {
                uint8_t data = img->data[w * j + i];
                for (uint8_t bit = 0; bit < 8; bit++) {
                    if (8 * j + bit < h) {
                        for (uint8_t my = 0; my < mult; my++)
                            st7793SendData(data & 0x01 ? color : bgColor);
                        data >>= 1;
                    }
                }
            }
        }
    }

    SET(ST7793_CS);
}

#include "st7920.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../../functions.h"

#define ST7920_WIDTH                128
#define ST7920_HEIGHT               64

static DispDriver drv = {
    .width = ST7920_WIDTH,
    .height = ST7920_HEIGHT,
    .drawPixel = st7920DrawPixel,
};

static uint8_t fb[ST7920_WIDTH / 4][ST7920_HEIGHT / 2];

static inline void st7920SetPort(uint8_t data) __attribute__((always_inline));
static void st7920SetPort(uint8_t data)
{
    ST7920_DATA_Port->BSRR = 0x00FF0000 | data;         // If port bits 7..0 are used
}

static inline void st7920SetDdrIn() __attribute__((always_inline));
static void st7920SetDdrIn()
{
    ST7920_DATA_Port->CRL = 0x88888888;                 // SET CNF=10, MODE=00 - Input pullup
}

static inline void st7920SetDdrOut() __attribute__((always_inline));
static void st7920SetDdrOut()
{
    ST7920_DATA_Port->CRL = 0x22222222;                 // Set CNF=00, MODE=10 - Output push-pull 2 MHz
}

static inline uint8_t st7920ReadPin(void) __attribute__((always_inline));
static uint8_t st7920ReadPin()
{
    return ST7920_DATA_Port->IDR & 0x00FF;              // Read 8-bit bus;
}

static void st7920WriteCmd(uint8_t cmd)
{
    _delay_us(100);

    CLR(ST7920_RS);
    st7920SetPort(cmd);

    SET(ST7920_E);
    _delay_us(1);
    CLR(ST7920_E);
}

void st7920IRQ(void)
{
    static uint8_t page = 0;
    static uint8_t phase = ST7920_PHASE_SET_PAGE;

    drv.bus = st7920ReadPin();                         // Read pins
    st7920SetDdrOut();                                  // Set data lines as outputs

    if (phase == ST7920_PHASE_SET_PAGE) {               // Phase 1 (Y)
        CLR(ST7920_RS);                                 // Go to command mode
        if (++page >= 32)
            page = 0;
        st7920SetPort(ST7920_SET_GRAPHIC_RAM | page);   // Set Y
    } else if (phase == ST7920_PHASE_SET_ADDR) {        // Phase 2 (X)
        st7920SetPort(ST7920_SET_GRAPHIC_RAM);          // Set X
    } else {                                            // Phase 3 (32 bytes of data)
        st7920SetPort(fb[phase][page]);
    }

    SET(ST7920_E);                                      // Strob
    _delay_us(1);
    CLR(ST7920_E);

    // Prepare to read pins
    st7920SetPort(0xFF);                                // Pull-up data lines
    st7920SetDdrIn();                                   // Set data lines as inputs

    if (++phase > ST7920_PHASE_SET_ADDR) {
        phase = 0;
        SET(ST7920_RS);                                 // Go to data mode
    }
}

void st7920Init(DispDriver **driver)
{
    *driver = &drv;

    // Stay always in write mode
    CLR(ST7920_RW);
    // Switch display to parallel mode
    SET(ST7920_PSB);

    // Init display in graphics mode
    st7920WriteCmd(ST7920_FUNCTION | ST7920_8BIT);
    st7920WriteCmd(ST7920_DISPLAY | ST7920_DISPLAY_ON);
    st7920WriteCmd(ST7920_ENTRY_MODE | ST7920_INC_ADDR);
    st7920WriteCmd(ST7920_FUNCTION | ST7920_8BIT | ST7920_EXT_INSTR);
    st7920WriteCmd(ST7920_FUNCTION | ST7920_8BIT | ST7920_EXT_INSTR | ST7920_GRAPHIC);

    SET(ST7920_RS);                                 // Go to data mode
}

void st7920Clear()
{
    uint8_t i, j;

    for (i = 0; i < ST7920_WIDTH / 4; i++) {
        for (j = 0; j < ST7920_HEIGHT / 2; j++) {
            fb[i][j] = 0x00;
        }
    }
}

void st7920DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    uint8_t bit;

    if (x >= ST7920_WIDTH)
        return;
    if (y >= ST7920_HEIGHT)
        return;

    bit = 0x80 >> (x & 0x07);

    if (y >= 32)
        x += 128;

    if (color)
        fb[x >> 3][y & 0x1F] |= bit;
    else
        fb[x >> 3][y & 0x1F] &= ~bit;
}

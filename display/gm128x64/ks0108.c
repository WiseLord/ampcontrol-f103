#include "ks0108.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../../functions.h"

static DispDriver drv = {
    .width = KS0108_COLS,
    .height = KS0108_ROWS * 8,
    .drawPixel = ks0108DrawPixel,
};

#if defined(_KS0108B)                               // CS active level is 0 (WG12864B)
#define KS0108_SET_CS1()    CLR(KS0108_CS1); SET(KS0108_CS2)
#define KS0108_SET_CS2()    SET(KS0108_CS1); CLR(KS0108_CS2)
#define KS0108_SET_CS()     CLR(KS0108_CS2); CLR(KS0108_CS1)
#else                                               // CS active level is 1 (WG12864A)
#define KS0108_SET_CS1()    SET(KS0108_CS1); CLR(KS0108_CS2)
#define KS0108_SET_CS2()    CLR(KS0108_CS1); SET(KS0108_CS2)
#define KS0108_SET_CS()     SET(KS0108_CS1); SET(KS0108_CS2)
#endif

static uint8_t fb[KS0108_COLS * KS0108_CHIPS][KS0108_ROWS];

static inline void ks0108SetPort(uint8_t data) __attribute__((always_inline));
static inline void ks0108SetPort(uint8_t data)
{
    KS0108_DATA_Port->BSRR = 0x00FF0000 | data;     // If port bits 7..0 are used
}

static inline void ks0108SetDdrIn() __attribute__((always_inline));
static inline void ks0108SetDdrIn()
{
    KS0108_DATA_Port->CRL = 0x88888888;             // SET CNF=10, MODE=00 - Input pullup
}

static inline void ks0108SetDdrOut() __attribute__((always_inline));
static inline void ks0108SetDdrOut()
{
    KS0108_DATA_Port->CRL = 0x22222222;             // Set CNF=00, MODE=10 - Output push-pull 2 MHz
}

static inline uint8_t ks0108ReadPin(void) __attribute__((always_inline));
static inline uint8_t ks0108ReadPin(void)
{
    return KS0108_DATA_Port->IDR & 0x00FF;          // Read 8-bit bus;
}

static void ks0108WriteCmd(uint8_t cmd)
{
    _delay_us(50);

    CLR(KS0108_DI);
    ks0108SetPort(cmd);

    SET(KS0108_E);
    _delay_us(1);
    CLR(KS0108_E);
}

void ks0108IRQ(void)
{
    static uint8_t page = 0;
    static uint8_t phase = KS0108_PHASE_SET_PAGE;
    static uint8_t cs;

    drv.bus = ks0108ReadPin();                     // Read pins
    ks0108SetDdrOut();                              // Set data lines as outputs

    if (phase == KS0108_PHASE_SET_PAGE) {           // Phase 1 (Y)
        if (++page >= 8) {
            page = 0;
            if (++cs >= KS0108_CHIPS)
                cs = 0;
            switch (cs) {
            case 1:
                KS0108_SET_CS2();
                break;
            default:
                KS0108_SET_CS1();
                break;
            }
        }
        CLR(KS0108_DI);                             // Go to command mode
        ks0108SetPort(KS0108_SET_PAGE + page);
    } else if (phase == KS0108_PHASE_SET_ADDR) {    // Phase 2 (X)
        ks0108SetPort(KS0108_SET_ADDRESS + 0);
    } else {                                        // Phase 3 (32 bytes of data)
        ks0108SetPort(fb[phase + 64 * cs][page]);
    }

    SET(KS0108_E);                                  // Data strob
    _delay_us(1);
    CLR(KS0108_E);

    // Prepare to read pins
    ks0108SetPort(0xFF);                            // Set 1 (pull-up) on data lines
    ks0108SetDdrIn();                               // Set data lines as inputs

    if (++phase > KS0108_PHASE_SET_ADDR) {
        phase = 0;
        SET(KS0108_DI);                             // Go to data mode
    }
}

void ks0108Init(DispDriver **driver)
{
    *driver = &drv;

    // Stay always in write mode
    CLR(KS0108_RW);

    // Init both controller
    KS0108_SET_CS();
    ks0108WriteCmd(KS0108_DISPLAY_START_LINE);
    ks0108WriteCmd(KS0108_DISPLAY_ON);
    LL_mDelay(10);

    SET(KS0108_DI);                             // Go to data mode
}

void ks0108Clear(void)
{
    uint8_t i, j;

    for (i = 0; i < KS0108_COLS * KS0108_CHIPS; i++) {
        for (j = 0; j < KS0108_ROWS; j++) {
            fb[i][j] = 0x00;
        }
    }
}

void ks0108DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    uint8_t bit;

    if (x >= KS0108_COLS * KS0108_CHIPS)
        return;
    if (y >= KS0108_ROWS * 8)
        return;

    bit = 1 << (y & 0x07);

    if (color)
        fb[x][y >> 3] |= bit;
    else
        fb[x][y >> 3] &= ~bit;
}

#include "ssd1306.h"
#include "ssd1306_regs.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../../i2c.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_i2c.h>
#include <stm32f1xx_ll_rcc.h>

#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01
#define I2C_SPEEDCLOCK                          400000
#define I2C_DUTYCYCLE                           LL_I2C_DUTYCYCLE_2

#define SSD1306_WIDTH                   128
#define SSD1306_HEIGHT                  64
#define SSD1306_BUFFERSIZE              (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static DispDriver drv = {
    .width = SSD1306_WIDTH,
    .height = SSD1306_HEIGHT,
    .drawPixel = ssd1306DrawPixel,
    .updateFB = ssd1306UpdateFb,
};

static uint8_t fb[SSD1306_BUFFERSIZE];
static uint8_t ssd1306Addr = SSD1306_I2C_ADDR_0;

static const uint8_t initSeq[] = {
    SSD1306_DISPLAY_OFF,
    SSD1306_SETDISPLAYCLOCKDIV,
    0x80,
    SSD1306_SETMULTIPLEX,
    (SSD1306_HEIGHT - 1),
    SSD1306_SETDISPLAYOFFSET,
    0x00,
    SSD1306_SETSTARTLINE | 0x00,
    SSD1306_MEMORYMODE,
#ifdef SSD1306_USE_PAGE_ADDRESSING
    SSD1306_MEMORYMODE_PAGE,
#else
    SSD1306_MEMORYMODE_HORISONTAL,
#endif
#ifdef SSD1306_ROTATE_180
    SSD1306_SEGREMAP_ON,
    SSD1306_COMSCANDEC,
#else
    SSD1306_SEGREMAP_OFF,
    SSD1306_COMSCANINC,
#endif
    SSD1306_SETCOMPINS,
#if SSD1306_HEIGHT == 64
    0x12,
#else
    0x02,
#endif
    SSD1306_SETCONTRAST,
    0x8F,
    SSD1306_SETPRECHARGE,
    0x1F,
    SSD1306_SETVCOMDETECT,
    0x40,
    SSD1306_ENTDISPLAY_RAM,
    SSD1306_NORMALDISPLAY,
    SSD1306_CHARGEPUMP,
    0x14,
    SSD1306_SCROLL_DEACTIVATE,
    SSD1306_DISPLAY_ON,
};

#ifndef SSD1306_USE_PAGE_ADDRESSING
static const uint8_t dispAreaSeq[] = {
    SSD1306_COLUMNADDR,
    0x00,
    0x7F,
    SSD1306_PAGEADDR,
    0x00,
    0x07,
};
#endif

void ssd1306Init(DispDriver **driver)
{
    *driver = &drv;

    // Configure Hardware I2C
    i2cInit(I2C_LCD, 400000);

    uint8_t i;

    i2cBegin(I2C_LCD, ssd1306Addr);
    i2cSend(I2C_LCD, SSD1306_I2C_COMMAND);

    for (i = 0; i < sizeof(initSeq); i++) {
        i2cSend(I2C_LCD, initSeq[i]);
    }

    i2cTransmit(I2C_LCD, true);
}

void ssd1306UpdateFb()
{
    uint16_t i;
    uint8_t *fbP = fb;

#ifdef SSD1306_USE_PAGE_ADDRESSING
    uint8_t page;
    for (page = 0; page < 8; page++) {
        i2cBegin(I2C_LCD, ssd1306Addr);
        i2cSend(I2C_LCD, SSD1306_I2C_COMMAND);
        i2cSend(I2C_LCD, SSD1306_SETLOWCOLUMN);
        i2cSend(I2C_LCD, SSD1306_SETHIGHCOLUMN);
        i2cSend(I2C_LCD, SSD1306_PAGE_START + page);
        i2cTransmit(I2C_LCD, true);

        i2cBegin(I2C_LCD, ssd1306Addr);
        i2cSend(I2C_LCD, SSD1306_I2C_DATA_SEQ);
        for (i = 0; i < SSD1306_WIDTH; i++) {
            i2cSend(I2C_LCD, *fbP++);
        }
        i2cTransmit(I2C_LCD, true);
    }
#else
    i2cBegin(I2C_LCD, ssd1306Addr);
    i2cSend(I2C_LCD, SSD1306_I2C_COMMAND);

    for (i = 0; i < sizeof(dispAreaSeq); i++)
        i2cSend(I2C_LCD, dispAreaSeq[i]);

    i2cTransmit(I2C_LCD, true);

    i2cBegin(I2C_LCD, ssd1306Addr);
    i2cSend(I2C_LCD, SSD1306_I2C_DATA_SEQ);

    for (i = 0; i < SSD1306_BUFFERSIZE; i++)
        i2cSend(I2C_LCD, *fbP++);

    i2cTransmit(I2C_LCD, true);
#endif
}

void ssd1306DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    uint8_t bit;
    uint8_t *fbP;

    if (x >= 128)
        return;
    if (y >= 64)
        return;

    fbP = &fb[(y >> 3) * 128 + x];

    bit = 1 << (y & 0x07);

    if (color)
        *fbP |= bit;
    else
        *fbP &= ~bit;
}

void ssd1306Clear()
{
    uint16_t i;
    uint8_t *fbP = fb;

    for (i = 0; i < SSD1306_BUFFERSIZE; i++)
        *fbP++ = 0x00;
}

void ssd1306SetBrightness(uint8_t br)
{
    uint8_t rawBr = 255;

    if (br < SSD1306_MAX_BRIGHTNESS)
        rawBr = br * 8;

    i2cBegin(I2C_LCD, ssd1306Addr);
    i2cSend(I2C_LCD, SSD1306_I2C_COMMAND);

    i2cSend(I2C_LCD, SSD1306_SETCONTRAST);
    i2cSend(I2C_LCD, rawBr);

    i2cTransmit(I2C_LCD, true);
}

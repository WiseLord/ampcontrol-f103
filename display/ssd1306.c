#include "ssd1306.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_i2c.h>
#include <stm32f1xx_ll_rcc.h>

#include "../pins.h"
#include "../functions.h"

#include "gm128x64.h"

#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01
#define I2C_SPEEDCLOCK                          400000
#define I2C_DUTYCYCLE                           LL_I2C_DUTYCYCLE_2

static GlcdDriver glcd = {
    .clear = ssd1306Clear,
    .drawPixel = ssd1306DrawPixel,
    .drawFontChar = glcdDrawFontChar,
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

static void _I2CStart(uint8_t addr)
{
    LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_ACK);

    LL_I2C_GenerateStartCondition(I2C2);
    while (!LL_I2C_IsActiveFlag_SB(I2C2));

    LL_I2C_TransmitData8(I2C2, addr | I2C_REQUEST_WRITE);

    while (!LL_I2C_IsActiveFlag_ADDR(I2C2));
    LL_I2C_ClearFlag_ADDR(I2C2);
}

static void _I2CWriteByte(uint8_t data)
{
    while (!LL_I2C_IsActiveFlag_TXE(I2C2));

    LL_I2C_TransmitData8(I2C2, data);
}

static void _I2CStop()
{
    while (!LL_I2C_IsActiveFlag_TXE(I2C2));

    LL_I2C_GenerateStopCondition(I2C2);
}

static void ssd1306InitI2C()
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

    LL_I2C_Disable(I2C2);

    LL_I2C_DisableOwnAddress2(I2C2);
    LL_I2C_DisableGeneralCall(I2C2);
    LL_I2C_EnableClockStretching(I2C2);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.ClockSpeed = 400000;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C2, &I2C_InitStruct);

    LL_I2C_SetOwnAddress2(I2C2, 0);

    LL_I2C_Enable(I2C2);
}

void ssd1306Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gm128x64Init(*driver);

    // Configure Hardware I2C
    ssd1306InitI2C();

    uint8_t i;

    _I2CStart(ssd1306Addr);
    _I2CWriteByte(SSD1306_I2C_COMMAND);

    for (i = 0; i < sizeof(initSeq); i++) {
        _I2CWriteByte(initSeq[i]);
    }

    _I2CStop();
}

void ssd1306UpdateFb()
{
    uint16_t i;
    uint8_t *fbP = fb;

#ifdef SSD1306_USE_PAGE_ADDRESSING
    uint8_t page;
    for (page = 0; page < 8; page++) {
        _I2CStart(ssd1306Addr);
        _I2CWriteByte(SSD1306_I2C_COMMAND);
        _I2CWriteByte(SSD1306_SETLOWCOLUMN);
        _I2CWriteByte(SSD1306_SETHIGHCOLUMN);
        _I2CWriteByte(SSD1306_PAGE_START + page);
        _I2CStop();

        _I2CStart(ssd1306Addr);
        _I2CWriteByte(SSD1306_I2C_DATA_SEQ);
        for (i = 0; i < SSD1306_WIDTH; i++) {
            _I2CWriteByte(*fbP++);
        }
        _I2CStop();
    }
#else
    _I2CStart(ssd1306Addr);
    _I2CWriteByte(SSD1306_I2C_COMMAND);

    for (i = 0; i < sizeof(dispAreaSeq); i++)
        _I2CWriteByte(dispAreaSeq[i]);

    _I2CStop();

    _I2CStart(ssd1306Addr);
    _I2CWriteByte(SSD1306_I2C_DATA_SEQ);

    for (i = 0; i < SSD1306_BUFFERSIZE; i++)
        _I2CWriteByte(*fbP++);

    _I2CStop();
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

    _I2CStart(ssd1306Addr);
    _I2CWriteByte(SSD1306_I2C_COMMAND);

    _I2CWriteByte(SSD1306_SETCONTRAST);
    _I2CWriteByte(rawBr);

    _I2CStop();
}

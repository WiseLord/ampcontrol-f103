#include "dispdrv.h"

#include <stdbool.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>
#include "../pins.h"

#ifdef _DISP_8BIT
static volatile bool bus_requested = false;
#endif
static volatile int8_t brightness;
static volatile uint8_t busData;

#define BUS_MODE_OUT        0x33333333  // CNF=0b00, MODE=0b11 => Output push-pull 50 MHz
#define BUS_MODE_IN         0x88888888  // CNF=0b10, MODE=0b00 - Input pullup

#ifdef _DISP_SPI
static void dispdrvInitSPI()
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    LL_SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(SPI2, &SPI_InitStruct);

    LL_SPI_Enable(SPI2);
}
#endif

__attribute__((always_inline))
static inline void dispdrvSendByte(uint8_t data)
{
#ifdef _DISP_SPI
    while (!LL_SPI_IsActiveFlag_TXE(SPI2));
    LL_SPI_TransmitData8(SPI2, data);
#else
#if defined(_DISP_HI_BYTE) && !defined(_DISP_16BIT)
    DISP_DATA_HI_Port->BSRR = 0xFF000000 | (data << 8);
#endif
#if defined(_DISP_LO_BYTE) || defined(_DISP_16BIT)
    DISP_DATA_LO_Port->BSRR = 0x00FF0000 | data;
#endif
    CLR(DISP_WR);
    SET(DISP_WR);
#endif
}

#if defined(_DISP_8BIT) || defined(_DISP_16BIT)

__attribute__((always_inline))
static inline void dispdrvBusIn(void)
{
#ifdef _DISP_HI_BYTE
    DISP_DATA_HI_Port->BSRR = 0x0000FF00;   // Set HIGH level on all data lines
    DISP_DATA_HI_Port->CRH = BUS_MODE_IN;
#endif
#ifdef _DISP_LO_BYTE
    DISP_DATA_LO_Port->BSRR = 0x000000FF;   // Set HIGH level on all data lines
    DISP_DATA_LO_Port->CRL = BUS_MODE_IN;
#endif
}

__attribute__((always_inline))
static inline void dispdrvBusOut(void)
{
#ifdef _DISP_HI_BYTE
    DISP_DATA_HI_Port->CRH = BUS_MODE_OUT;
#endif
#ifdef _DISP_LO_BYTE
    DISP_DATA_LO_Port->CRL = BUS_MODE_OUT;
#endif
}

__attribute__((always_inline))
static inline uint32_t dispDrvGetBusMode(void)
{
#ifdef _DISP_HI_BYTE
    return DISP_DATA_HI_Port->CRH;
#endif
#ifdef _DISP_LO_BYTE
    return DISP_DATA_LO_Port->CRL;
#endif
}

#endif // _DISP_8BIT

__attribute__((always_inline))
static inline void dispdrvReadInput(void)
{
#ifdef _DISP_HI_BYTE
    busData = (DISP_DATA_HI_Port->IDR & 0xFF00) >> 8;
#endif
#ifdef _DISP_LO_BYTE
    busData = DISP_DATA_LO_Port->IDR & 0x00FF;
#endif
}

__attribute__((always_inline))
static inline void dispdrvSendWord(uint16_t data)
{
#ifdef _DISP_16BIT
    DISP_DATA_HI_Port->BSRR = 0xFF000000 | (data & 0xFF00);
    DISP_DATA_LO_Port->BSRR = 0x00FF0000 | (data & 0x00FF);

    CLR(DISP_WR);
    SET(DISP_WR);
#else
#ifndef _DISP_SPI
    dispdrvBusOut();
#endif
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;
    dispdrvSendByte(dataH);
    dispdrvSendByte(dataL);
#ifdef _DISP_8BIT
    dispdrvBusIn();
#endif
#endif
#ifdef _DISP_8BIT
    if (bus_requested) {
        dispdrvReadInput();
        bus_requested = false;
    }
#endif
}

void dispdrvReset(void)
{
#ifdef _DISP_SPI
    SET(DISP_SPI_DC);
    dispdrvInitSPI();
#else
#ifdef _DISP_READ_ENABLED
    SET(DISP_RD);
#endif
    SET(DISP_WR);
    SET(DISP_RS);
    SET(DISP_CS);
#endif

#ifdef _DISP_RST_ENABLED
    CLR(DISP_RST);
    LL_mDelay(50);
    SET(DISP_RST);
#endif
    LL_mDelay(50);
}

void dispdrvInit(void)
{
    dispdrvReset();

    dispdrv.init();

    SET(DISP_BCKL);
}

void dispdrvPwm(void)
{
    static uint8_t br;

    if (++br >= LCD_BR_MAX)
        br = 0;

    if (br == brightness) {
        CLR(DISP_BCKL);
    } else if (br == 0) {
        SET(DISP_BCKL);
    }
}

void dispdrvSetBrightness(int8_t value)
{
    brightness = value;
}

uint8_t dispdrvGetBus(void)
{
    return ~busData;
}

void dispdrvBusIRQ(void)
{
#ifdef _DISP_8BIT
    if (dispDrvGetBusMode() == BUS_MODE_OUT) {
        bus_requested = true;               // Postpone read bus until in input mode
    } else {
        dispdrvReadInput();                 // Read bus immediately
    }
#endif
#if defined(_DISP_SPI)
    dispdrvReadInput();
#endif
}

void dispdrvSendData8(uint8_t data)
{
#ifdef _DISP_8BIT
    dispdrvBusOut();
#endif
    dispdrvSendByte(data);
#ifdef _DISP_8BIT
    dispdrvBusIn();
    if (bus_requested) {
        dispdrvReadInput();
        bus_requested = false;
    }
#endif
}

void dispdrvSendData16(uint16_t data)
{
    dispdrvSendWord(data);
}

void dispdrvSelectReg8(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

void dispdrvWriteReg8(uint8_t reg, uint8_t data)
{
    dispdrvSelectReg8(reg);
    dispdrvSendData8(data);
}

void dispdrvSelectReg16(uint16_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendWord(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

void dispdrvWriteReg16(uint16_t reg, uint16_t data)
{
    dispdrvSelectReg16(reg);
    dispdrvSendWord(data);
}

#ifdef _DISP_READ_ENABLED

static void dispdrvReadDelay(void)
{
    volatile uint32_t ticks = 50;
    while (ticks--);
}

__attribute__((always_inline))
static inline uint8_t dispdrvReadByte(void)
{
    uint8_t ret;

    CLR(DISP_RD);
    dispdrvReadDelay();
#ifdef _DISP_HI_BYTE
    ret = (DISP_DATA_HI_Port->IDR & 0xFF00) >> 8;
#else
    ret = (DISP_DATA_LO_Port->IDR & 0x00FF);
#endif
    SET(DISP_RD);

    return  ret;
}


uint16_t dispdrvReadData16(void)
{
    uint16_t ret = 0;

#ifndef _DISP_SPI
    dispdrvBusIn();
#endif

#if defined(_DISP_16BIT)
    CLR(DISP_RD);
    dispdrvReadDelay();
    ret |= DISP_DATA_HI_Port->IDR & 0xFF00;
    ret |= DISP_DATA_LO_Port->IDR & 0x00FF;
    SET(DISP_RD);
#elif defined(_DISP_8BIT)
    ret |= dispdrvReadByte();
    ret <<= 8;
    ret |= dispdrvReadByte();
#endif

#ifndef _DISP_SPI
    dispdrvBusOut();
#endif

    return ret;
}

void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs)
{
    CLR(DISP_CS);

    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);

    for (uint8_t i = 0; i < nArgs; i++) {
        args[i] = dispdrvReadData16();
    }

    SET(DISP_CS);
}

#endif

void dispdrvDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, 1, 1);

    dispdrvSendWord(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void dispdrvDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, w, h);

    for (int32_t i = 0; i < w * h; i++) {
        dispdrvSendWord(color);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    dispdrv.setWindow(x, y, w, h);

    for (uint16_t i = 0; i < w; i++) {
        for (uint16_t j = 0; j < (h + 7) / 8; j++) {
            uint8_t data = img->data[w * j + i];
            for (uint8_t bit = 0; bit < 8; bit++) {
                if (8 * j + bit < h) {
                    dispdrvSendWord(data & 0x01 ? color : bgColor);
                    data >>= 1;
                }
            }
        }
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

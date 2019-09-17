#include "dispdrv.h"

#include <stdbool.h>

static volatile bool busBusy;
static volatile int8_t brightness;
static volatile uint8_t busData;

__attribute__((always_inline))
static inline void dispdrvSendByte(uint8_t data)
{
#ifdef _DISP_SPI
    spiSendByte(SPI_DISPLAY, data);
#else
#if defined(_DISP_16BIT)
    WRITE_BYTE(DISP_DATA_HI, 0x00);
    WRITE_BYTE(DISP_DATA_LO, data);
#elif defined(_DISP_8BIT)
    WRITE_BYTE(DISP_DATA, data);
#endif
    __asm volatile ("nop");
    CLR(DISP_WR);
    __asm volatile ("nop");
    SET(DISP_WR);
#endif
}

__attribute__((always_inline))
static inline uint8_t  dispdrvReadBus(void)
{
    uint8_t ret = 0;

#if defined(_DISP_16BIT)
    ret = READ_BYTE(DISP_DATA_LO);
#else
#ifdef DISP_DATA_Port
    ret = READ_BYTE(DISP_DATA);
#endif
#endif

    return ret;
}

__attribute__((always_inline))
static inline void dispdrvBusIn(void)
{
#if defined(_DISP_16BIT)
    WRITE_BYTE(DISP_DATA_HI, 0xFF);         // Set HIGH level on all data lines
    WRITE_BYTE(DISP_DATA_LO, 0xFF);
#ifdef _STM32F1
    DISP_DATA_HI_Port->CRH = 0x88888888;
    DISP_DATA_LO_Port->CRL = 0x88888888;
#endif
#ifdef _STM32F3
    DISP_DATA_HI_Port->MODER &= 0x0000FFFF;
    DISP_DATA_LO_Port->MODER &= 0xFFFF0000;
#endif
#elif defined(_DISP_8BIT)
    WRITE_BYTE(DISP_DATA, 0xFF);            // Set HIGH level on all data lines

#if IS_GPIO_LO(DISP_DATA)
#ifdef _STM32F1
    DISP_DATA_Port->CRL = 0x88888888;
#endif
#ifdef _STM32F3
    DISP_DATA_Port->MODER &= 0xFFFF0000;
#endif
#elif IS_GPIO_HI(DISP_DATA)
#ifdef _STM32F1
    DISP_DATA_Port->CRH = 0x88888888;
#endif
#ifdef _STM32F3
    DISP_DATA_Port->MODER &= 0x0000FFFF;
#endif
#endif
#endif
    busBusy = false;
}

__attribute__((always_inline))
static inline void dispdrvBusOut(void)
{
    if (!busBusy) {
        busData = dispdrvReadBus();
        busBusy = true;
    }
#if defined(_DISP_16BIT)
#ifdef _STM32F1
    DISP_DATA_HI_Port->CRH = 0x33333333;
    DISP_DATA_LO_Port->CRL = 0x33333333;
#endif
#ifdef _STM32F3
    DISP_DATA_HI_Port->MODER &= 0x0000FFFF;
    DISP_DATA_HI_Port->MODER |= 0x55550000;
    DISP_DATA_LO_Port->MODER &= 0xFFFF0000;
    DISP_DATA_LO_Port->MODER |= 0x55550000;
#endif
#elif defined(_DISP_8BIT)
#if IS_GPIO_LO(DISP_DATA)
#ifdef _STM32F1
    DISP_DATA_Port->CRL = 0x33333333;
#endif
#ifdef _STM32F3
    DISP_DATA_Port->MODER &= 0xFFFF0000;
    DISP_DATA_Port->MODER |= 0x00005555;
#endif
#endif
#if IS_GPIO_HI(DISP_DATA)
#ifdef _STM32F1
    DISP_DATA_Port->CRH = 0x33333333;
#endif
#ifdef _STM32F3
    DISP_DATA_Port->MODER &= 0x0000FFFF;
    DISP_DATA_Port->MODER |= 0x55550000;
#endif
#endif
#endif
}

__attribute__((always_inline))
static inline void dispdrvSendWord(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;
#ifdef _DISP_16BIT
    WRITE_BYTE(DISP_DATA_HI, dataH);
    WRITE_BYTE(DISP_DATA_LO, dataL);

    __asm volatile ("nop");
    CLR(DISP_WR);
    __asm volatile ("nop");
    SET(DISP_WR);
#else
#ifndef _DISP_SPI
    dispdrvBusOut();
#endif
    dispdrvSendByte(dataH);
    dispdrvSendByte(dataL);
#ifndef _DISP_SPI
    dispdrvBusIn();
#endif

#endif // _DISP_16BIT
}

#ifdef _COLOR_24BIT
__attribute__((always_inline))
static inline void dispdrvSendTriplet(uint16_t data)
{
    uint8_t dataH = (data & 0xF800) >> 8;
    uint8_t dataM = (data & 0x07E0) >> 5;
    uint8_t dataL = (data & 0x001F) << 3;

#ifndef _DISP_SPI
    dispdrvBusOut();
#endif

    dispdrvSendByte(dataH);
    dispdrvSendByte(dataM);
    dispdrvSendByte(dataL);

#ifndef _DISP_SPI
    dispdrvBusIn();
#endif
}
#endif

__attribute__((always_inline))
static inline void dispdrvSendColor(uint16_t data)
{
#ifdef _COLOR_24BIT
    dispdrvSendTriplet(color);
#else
    dispdrvSendWord(data);
#endif
}

void dispdrvReset(void)
{
#ifdef _DISP_SPI
    spiInit(SPI_DISPLAY, false);
#else
    SET(DISP_CS);
#ifdef _DISP_READ_ENABLED
    SET(DISP_RD);
#endif
    SET(DISP_WR);
#endif
    SET(DISP_RS);
#ifdef _DISP_RST_ENABLED
    CLR(DISP_RST);
    utilmDelay(50);
    SET(DISP_RST);
#endif
    utilmDelay(50);
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
    if (!busBusy) {
        busData = dispdrvReadBus();
    }
}

void dispdrvSendData8(uint8_t data)
{
#ifdef _DISP_8BIT
    dispdrvBusOut();
#endif
    dispdrvSendByte(data);
#ifdef _DISP_8BIT
    dispdrvBusIn();
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

__attribute__((always_inline))
static inline void dispdrvReadDelay(void)
{
    volatile uint32_t ticks = 50;
    while (ticks--);
}

__attribute__((always_inline))
static inline uint8_t dispdrvReadByte(void)
{
    uint8_t ret = 0;

#ifdef DISP_RD_Port
    CLR(DISP_RD);
    dispdrvReadDelay();
    ret = dispdrvReadBus();
    SET(DISP_RD);
#endif

    return ret;
}

uint16_t dispdrvReadData16(void)
{
    uint16_t ret = 0;

#ifndef _DISP_SPI
    dispdrvBusIn();
#endif

#if defined(_DISP_16BIT)
#ifdef DISP_RD_Port
    CLR(DISP_RD);
    dispdrvReadDelay();
    ret |= READ_BYTE(DISP_DATA_HI) << 8;
    ret |= READ_BYTE(DISP_DATA_LO);
    SET(DISP_RD);
#endif
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

void dispdrvDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, 1, 1);

    dispdrvSendColor(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, w, h);

    for (int32_t i = 0; i < w * h; i++) {
        dispdrvSendColor(color);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h)
{
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, w, h);

    for (int16_t i = 0; i < w; i++) {
        for (int16_t j = 0; j < h; j++) {
            uint8_t data = img->data[img->width * ((j + yOft) >> 3) + i + xOft];
            if (j < h) {
                dispdrvSendColor(data & (1 << ((j + yOft) & 0x7)) ? color : bgColor);
            }
        }
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

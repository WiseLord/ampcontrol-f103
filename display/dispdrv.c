#include "dispdrv.h"

#include <stdbool.h>

static volatile bool bus_requested = false;
static volatile int8_t brightness;
static volatile uint8_t busData;

#ifdef _STM32F1
#define BUS_MODE_OUT        0x33333333  // CNF=0b00, MODE=0b11 => Output push-pull 50 MHz
#define BUS_MODE_IN         0x88888888  // CNF=0b10, MODE=0b00 - Input pullup
#endif
#ifdef _STM32F3
#if IS_GPIO_LO(DISP_DATA_HI)
#define BUS_MODE_OUT        0x55550000
#define BUS_MODE_IN         0x00000000
#else
#define BUS_MODE_OUT        0x00005555
#define BUS_MODE_IN         0x00000000
#endif
#endif

__attribute__((always_inline))
static inline void dispdrvSendByte(uint8_t data)
{
#ifdef _DISP_SPI
    spiSendByte(SPI_DISPLAY, data);
#else
#if IS_GPIO_LO(DISP_DATA_LO)
    DISP_DATA_LO_Port->BSRR = 0x00FF0000 | data;
#elif IS_GPIO_HI(DISP_DATA_HI)
    DISP_DATA_HI_Port->BSRR = 0xFF000000 | (uint16_t)(data << 8);
#endif
    __asm volatile ("nop");
    CLR(DISP_WR);
    __asm volatile ("nop");
    SET(DISP_WR);
#endif
}

__attribute__((always_inline))
static inline void dispdrvReadInput(void)
{
#if IS_GPIO_LO(DISP_DATA_LO)
    busData = DISP_DATA_LO_Port->IDR & 0x00FF;
#elif IS_GPIO_HI(DISP_DATA_HI)
    busData = (DISP_DATA_HI_Port->IDR & 0xFF00) >> 8;
#endif
}

#if defined(_DISP_8BIT) || defined(_DISP_16BIT)

__attribute__((always_inline))
static inline void dispdrvBusIn(void)
{
#if IS_GPIO_LO(DISP_DATA_LO)
    DISP_DATA_LO_Port->BSRR = 0x000000FF;   // Set HIGH level on all data lines
#ifdef _STM32F1
    DISP_DATA_LO_Port->CRL = BUS_MODE_IN;
#endif
#ifdef _STM32F3
    DISP_DATA_LO_Port->MODER &= 0xFFFF0000;
    DISP_DATA_LO_Port->MODER |= BUS_MODE_IN;
#endif
#endif
#if IS_GPIO_HI(DISP_DATA_HI)
    DISP_DATA_HI_Port->BSRR = 0x0000FF00;   // Set HIGH level on all data lines
#ifdef _STM32F1
    DISP_DATA_HI_Port->CRH = BUS_MODE_IN;
#endif
#ifdef _STM32F3
    DISP_DATA_HI_Port->MODER &= 0x0000FFFF;
    DISP_DATA_HI_Port->MODER |= BUS_MODE_IN;
#endif
#endif
}

__attribute__((always_inline))
static inline void dispdrvBusOut(void)
{
    if (bus_requested) {
        dispdrvReadInput();
        bus_requested = false;
    }
#if IS_GPIO_LO(DISP_DATA_LO)
#ifdef _STM32F1
    DISP_DATA_LO_Port->CRL = BUS_MODE_OUT;
#endif
#ifdef _STM32F3
    DISP_DATA_LO_Port->MODER &= 0xFFFF0000;
    DISP_DATA_LO_Port->MODER |= BUS_MODE_OUT;
#endif
#endif
#if IS_GPIO_HI(DISP_DATA_HI)
#ifdef _STM32F1
    DISP_DATA_HI_Port->CRH = BUS_MODE_OUT;
#endif
#ifdef _STM32F3
    DISP_DATA_HI_Port->MODER &= 0x0000FFFF;
    DISP_DATA_HI_Port->MODER |= BUS_MODE_OUT;
#endif
#endif
}

__attribute__((always_inline))
static inline uint32_t dispDrvGetBusMode(void)
{
#if IS_GPIO_LO(DISP_DATA_LO)
#ifdef _STM32F1
    return DISP_DATA_LO_Port->CRL;
#endif
#ifdef _STM32F3
    return DISP_DATA_LO_Port->MODER & 0x0000FFFF;
#endif
#elif IS_GPIO_HI(DISP_DATA_HI)
#ifdef _STM32F1
    return DISP_DATA_HI_Port->CRH;
#endif
#ifdef _STM32F3
    return DISP_DATA_HI_Port->MODER & 0xFFFF0000;
#endif
#else
    return BUS_MODE_IN;
#endif
}

#endif // _DISP_8BIT || _DISP_16BIT

__attribute__((always_inline))
static inline void dispdrvSendWord(uint16_t data)
{
#ifdef _DISP_16BIT
    DISP_DATA_HI_Port->BSRR = 0xFF000000 | (data & 0xFF00);
    DISP_DATA_LO_Port->BSRR = 0x00FF0000 | (data & 0x00FF);

    CLR(DISP_WR);
    SET(DISP_WR);
#else
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;
#ifdef _DISP_8BIT
    dispdrvBusOut();
#endif
    dispdrvSendByte(dataH);
    dispdrvSendByte(dataL);
#ifdef _DISP_8BIT
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

#ifdef _DISP_8BIT
    dispdrvBusOut();
#endif

    dispdrvSendByte(dataH);
    dispdrvSendByte(dataM);
    dispdrvSendByte(dataL);

#ifdef _DISP_8BIT
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
    SET(DISP_SPI_DC);
    spiInit(SPI_DISPLAY);
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
#if defined(_DISP_SPI)
    dispdrvReadInput();
#else
    if (dispDrvGetBusMode() == BUS_MODE_OUT) {
        bus_requested = true;               // Postpone read bus until in input mode
    } else {
        dispdrvReadInput();                 // Read bus immediately
    }
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
#if IS_GPIO_LO(DISP_DATA_LO)
    ret = (DISP_DATA_LO_Port->IDR & 0x00FF);
#else
    ret = (DISP_DATA_HI_Port->IDR & 0xFF00) >> 8;
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

#include "dispdrv.h"

#include <stdbool.h>

static volatile bool busBusy;
static volatile uint8_t busData;

#ifdef _DISP_FB

typedef struct {
    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;
    int16_t x;
    int16_t y;
} FbArea;

static FbArea fbArea;

__attribute__((always_inline))
static inline void fbSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    fbArea.xMin = x;
    fbArea.yMin = y;
    fbArea.xMax = x + w;
    fbArea.yMax = y + h;
    fbArea.x = x;
    fbArea.y = y;
}

__attribute__((always_inline))
static inline void fbSetPixel(int16_t x, int16_t y, color_t color)
{
    if (dispdrv.fbSetPixel) {
        dispdrv.fbSetPixel(x, y, color);
    }
}

__attribute__((always_inline))
static inline void fbShiftPos()
{
    if (++fbArea.y >= fbArea.yMax) {
        fbArea.y = fbArea.yMin;
        if (++fbArea.x >= fbArea.xMax) {
            fbArea.x = fbArea.xMin;
        }
    }
}

#endif // _DISP_FB

__attribute__((always_inline))
static inline void dispdrvSendByte(uint8_t data)
{
#ifdef _DISP_SPI
    DISP_SPI_SEND_BYTE(data);
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

#ifndef _DISP_SPI

__attribute__((always_inline))
static inline void dispdrvBusIn(void)
{
#if defined(_DISP_16BIT)
    WRITE_BYTE(DISP_DATA_HI, 0xFF);         // Set HIGH level on all data lines
    WRITE_BYTE(DISP_DATA_LO, 0xFF);
    IN_BYTE(DISP_DATA_HI);
    IN_BYTE(DISP_DATA_LO);
#elif defined(_DISP_8BIT)
    WRITE_BYTE(DISP_DATA, 0xFF);            // Set HIGH level on all data lines
    IN_BYTE(DISP_DATA);
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
    OUT_BYTE(DISP_DATA_HI);
    OUT_BYTE(DISP_DATA_LO);
#elif defined(_DISP_8BIT)
    OUT_BYTE(DISP_DATA);
#endif
}

#endif // _DISP_SPI

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
static inline void dispdrvSendColor(color_t data)
{
#ifdef _DISP_FB
    fbSetPixel(fbArea.x, fbArea.y, data);
    fbShiftPos();
#else
#ifdef _COLOR_24BIT
    dispdrvSendTriplet(color);
#else
    dispdrvSendWord(data);
#endif
#endif
}

void dispdrvReset(void)
{
#ifdef _DISP_BCKL_ENABLED
    CLR(DISP_BCKL);
#endif

#ifdef _DISP_SPI
    DISP_SPI_INIT();
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
    DISP_MDELAY(50);
    SET(DISP_RST);
#endif
    DISP_MDELAY(50);
}

static void dispdrvInitPins(void)
{
    OUT_PIN(DISP_CS);
    OUT_PIN(DISP_RS);

#ifdef _DISP_RST_ENABLED
    OUT_PIN(DISP_RST);
#endif

#ifndef _DISP_SPI
    OUT_PIN(DISP_WR);
#endif

#ifdef _DISP_READ_ENABLED
    OUT_PIN(DISP_RD);
#endif

#ifdef _DISP_BCKL_ENABLED
    OUT_PIN(DISP_BCKL);
#endif

#ifndef _DISP_SPI
    dispdrvBusOut();
#endif
}

void dispdrvInit(void)
{
    dispdrvInitPins();

    dispdrvReset();

    dispdrv.init();
}

uint8_t dispdrvGetBus(void)
{
    if (!busBusy) {
        busData = dispdrvReadBus();
    }

    return busData;
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

#ifdef DISP_RD_Port

__attribute__((always_inline))
static inline void dispdrvReadDelay(void)
{
    volatile uint32_t ticks = 50;
    while (ticks--);
}

#endif // DISP_RD_Port

#ifdef _DISP_8BIT

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

#endif // _DISP_8BIT

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

__attribute__((always_inline))
static inline void dispdrvSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
#ifdef _DISP_FB
    fbSetWindow(x, y, w, h);
#else
    if (dispdrv.setWindow) {
        dispdrv.setWindow(x, y, w, h);
    }
#endif
}

void dispdrvDrawPixel(int16_t x, int16_t y, color_t color)
{
#ifdef _DISP_FB
    fbSetPixel(x, y, color);
#else
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, 1, 1);

    dispdrvSendColor(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color)
{
#ifndef _DISP_FB
    CLR(DISP_CS);
#endif

    dispdrvSetWindow(x, y, w, h);

    for (int32_t i = 0; i < w * h; i++) {
        dispdrvSendColor(color);
    }

#ifndef _DISP_FB
    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

void dispdrvDrawVertGrad(int16_t x, int16_t y, int16_t w, int16_t h, color_t *gr)
{
#ifndef _DISP_FB
    CLR(DISP_CS);
#endif

    dispdrvSetWindow(x, y, w, h);

    for (int32_t i = 0; i < w; i++) {
        color_t *color = gr;
        for (int32_t j = 0; j < h; j++) {
            dispdrvSendColor(*color++);
        }
    }

#ifndef _DISP_FB
    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

void dispdrvDrawImage(tImage *img, bool portrate, int16_t x, int16_t y,
                      color_t color, color_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h)
{
#ifndef _DISP_FB
    CLR(DISP_CS);
#endif

    if (portrate) {
        dispdrvSetWindow(y, dispdrv.height - w - x, h, w);

        for (int16_t j = 0; j < h; j++) {
            for (int16_t i = w - 1; i >= 0; i--) {
                uint8_t data = img->data[img->width * ((j + yOft) >> 3) + i + xOft];
                if (j < h) {
                    dispdrvSendColor(data & (1 << ((j + yOft) & 0x7)) ? color : bgColor);
                }
            }
        }
    } else {
        dispdrvSetWindow(x, y, w, h);

        for (int16_t i = 0; i < w; i++) {
            for (int16_t j = 0; j < h; j++) {
                uint8_t data = img->data[img->width * ((j + yOft) >> 3) + i + xOft];
                if (j < h) {
                    dispdrvSendColor(data & (1 << ((j + yOft) & 0x7)) ? color : bgColor);
                }
            }
        }
    }


#ifndef _DISP_FB
    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

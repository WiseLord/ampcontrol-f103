#include "lph9157.h"

#include "../dispcanvas.h"

#include "../../pins.h"
#include "../../functions.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_spi.h>

#define LPH9157_WIDTH           132
#define LPH9157_HEIGHT          176
#define LPH9157_PIXELS          (LPH9157_WIDTH * LPH9157_HEIGHT)

#define TX_BUSY()               (LL_SPI_IsActiveFlag_BSY(SPI1) || !LL_SPI_IsActiveFlag_TXE(SPI1))

static GlcdDriver glcd = {
    .clear = lph9157Clear,
    .drawPixel = lph9157DrawPixel,
    .drawRectangle = lph9157DrawRectangle,
    .drawImage = lph9157DrawImage,
};

static inline void lph9157SendSPI(uint8_t data) __attribute__((always_inline));
static inline void lph9157SendSPI(uint8_t data)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
}

static inline void lph9157SendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void lph9157SendCmd(uint8_t cmd)
{
    while (TX_BUSY());
    CLR(DISP_SPI_DC);
    LL_SPI_TransmitData8(SPI1, cmd);
    while (TX_BUSY());
    SET(DISP_SPI_DC);
}

static inline void lph9157SendData(uint16_t data) __attribute__((always_inline));
static inline void lph9157SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    lph9157SendSPI(dataH);
    lph9157SendSPI(dataL);
}

static void lph9157InitSeq(void)
{
    _delay_ms(50);

    CLR(DISP_SPI_CS);

    lph9157SendCmd(0x01);
    lph9157SendCmd(0x11);
    _delay_ms(20);
    lph9157SendCmd(0x3a);
    lph9157SendSPI(0x05);
    _delay_ms(20);
    lph9157SendCmd(0x36);
    lph9157SendSPI(0x40);
    lph9157SendCmd(0x29);

    while (TX_BUSY());
    SET(DISP_SPI_CS);
}

void lph9157SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    lph9157SendCmd(0x2a);
    lph9157SendSPI(y);
    lph9157SendSPI(y + h - 1);

    lph9157SendCmd(0x2b);
    lph9157SendSPI(x);
    lph9157SendSPI(x + w - 1);

    lph9157SendCmd(0x2C);
}

static void lph9157InitSPI()
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(SPI1, &SPI_InitStruct);

    LL_SPI_Enable(SPI1);
}

void lph9157Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc176x132Init(*driver);

    // Configure Hardware SPI
    lph9157InitSPI();


    SET(DISP_SPI_RST);

    // Init magic
    lph9157InitSeq();
    SET(DISP_SPI_LED);
}

void lph9157Clear(void)
{
    lph9157DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, glcd.canvas->color);
}

void lph9157Sleep(void)
{
    CLR(DISP_SPI_CS);
    lph9157SendCmd(0x10);
    lph9157SendCmd(0x28);
    while (TX_BUSY());
    SET(DISP_SPI_CS);
}

void lph9157Wakeup(void)
{
    CLR(DISP_SPI_CS);
    lph9157SendCmd(0x11);
    lph9157SendCmd(0x29);
    while (TX_BUSY());
    SET(DISP_SPI_CS);
}

void lph9157DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_SPI_CS);
    lph9157SetWindow(x, y, 1, 1);
    lph9157SendData(color);
    while (TX_BUSY());
    SET(DISP_SPI_CS);
}

void lph9157DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t colorH = color >> 8;
    uint8_t colorL = color & 0xFF;

    CLR(DISP_SPI_CS);

    lph9157SetWindow(x, y, w, h);

    for (uint16_t i = 0; i < w; i++) {
        for (uint16_t j = 0; j < h; j++) {
            lph9157SendSPI(colorH);
            lph9157SendSPI(colorL);
        }
    }

    while (TX_BUSY());
    SET(DISP_SPI_CS);
}

void lph9157DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(DISP_SPI_CS);

    lph9157SetWindow(x0, y0, w, h);

    DISPDRV_SEND_IMAGE(img, lph9157SendData);

    while (TX_BUSY());
    SET(DISP_SPI_CS);
}

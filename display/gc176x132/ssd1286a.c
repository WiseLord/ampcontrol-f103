#include "ssd1286a.h"

#include "../dispcanvas.h"

#include "../../pins.h"
#include "../../functions.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_spi.h>

#define SSD1286A_WIDTH           132
#define SSD1286A_HEIGHT          176
#define SSD1286A_PIXELS          (SSD1286A_WIDTH * SSD1286A_HEIGHT)

#define TX_BUSY()               (LL_SPI_IsActiveFlag_BSY(SPI1) || !LL_SPI_IsActiveFlag_TXE(SPI1))

static GlcdDriver glcd = {
    .clear = ssd1286aClear,
    .drawPixel = ssd1286aDrawPixel,
    .drawRectangle = ssd1286aDrawRectangle,
    .drawImage = ssd1286aDrawImage,
};

static inline void ssd1286aSendSPI(uint8_t data) __attribute__((always_inline));
static inline void ssd1286aSendSPI(uint8_t data)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
}

static inline void ssd1286aSendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void ssd1286aSendCmd(uint8_t cmd)
{
    while (TX_BUSY());
    CLR(SSD1286A_DC);
    LL_SPI_TransmitData8(SPI1, cmd);
    while (TX_BUSY());
    SET(SSD1286A_DC);
}

static inline void ssd1286aSendData(uint16_t data) __attribute__((always_inline));
static inline void ssd1286aSendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ssd1286aSendSPI(dataH);
    ssd1286aSendSPI(dataL);
}

static void ssd1286aInitSeq(void)
{
    _delay_ms(50);

    CLR(SSD1286A_CS);

    ssd1286aSendCmd(0x00);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x01);

    ssd1286aSendCmd(0x10);
    ssd1286aSendSPI(0x1f);
    ssd1286aSendSPI(0x92);

    ssd1286aSendCmd(0x11);
    ssd1286aSendSPI(0x61);
    ssd1286aSendSPI(0x1c);

    ssd1286aSendCmd(0x28);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x06);

    ssd1286aSendCmd(0x02);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x00);

    ssd1286aSendCmd(0x12);
    ssd1286aSendSPI(0x04);
    ssd1286aSendSPI(0x0f);

    ssd1286aSendCmd(0x01);
    ssd1286aSendSPI(0x30);
    ssd1286aSendSPI(0xb0);

    ssd1286aSendCmd(0x03);
    ssd1286aSendSPI(0x78);
    ssd1286aSendSPI(0x30);

    ssd1286aSendCmd(0x07);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x33);

    while (TX_BUSY());
    SET(SSD1286A_CS);
}


void ssd1286aSetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ssd1286aSendCmd(0x44);
    ssd1286aSendSPI(y + h - 1);
    ssd1286aSendSPI(y);

    ssd1286aSendCmd(0x47);
    ssd1286aSendSPI(x + w - 1);
    ssd1286aSendSPI(x);

    ssd1286aSendCmd(0x21);
    ssd1286aSendSPI(x);
    ssd1286aSendSPI(y);

    ssd1286aSendCmd(0x22);
}

static void ssd1286aInitSPI()
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

void ssd1286aInit(GlcdDriver **driver)
{
    *driver = &glcd;
    gc176x132Init(*driver);

    // Configure Hardware SPI
    ssd1286aInitSPI();

    CLR(SSD1286A_RST);
    _delay_ms(100);
    SET(SSD1286A_RST);

    // Init magic
    ssd1286aInitSeq();
    SET(SSD1286A_LED);
}

void ssd1286aClear(void)
{
    ssd1286aDrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, glcd.canvas->color);
}

void ssd1286aSleep(void)
{
    CLR(SSD1286A_CS);

    ssd1286aSendCmd(0x07);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x00);

    ssd1286aSendCmd(0x10);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x01);

    ssd1286aSendCmd(0x11);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x00);

    ssd1286aSendCmd(0x12);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x00);

    while (TX_BUSY());
    SET(SSD1286A_CS);
}

void ssd1286aWakeup(void)
{
    CLR(SSD1286A_CS);

    ssd1286aSendCmd(0x10);
    ssd1286aSendSPI(0x1f);
    ssd1286aSendSPI(0x92);

    _delay_ms(20);

    ssd1286aSendCmd(0x11);
    ssd1286aSendSPI(0x61);
    ssd1286aSendSPI(0x1c);

    _delay_ms(20);

    ssd1286aSendCmd(0x12);
    ssd1286aSendSPI(0x04);
    ssd1286aSendSPI(0x0f);

    _delay_ms(20);

    ssd1286aSendCmd(0x07);
    ssd1286aSendSPI(0x00);
    ssd1286aSendSPI(0x33);

    while (TX_BUSY());
    SET(SSD1286A_CS);
}

void ssd1286aDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(SSD1286A_CS);
    ssd1286aSetWindow(x, y, 1, 1);
    ssd1286aSendData(color);
    while (TX_BUSY());
    SET(SSD1286A_CS);
}

void ssd1286aDrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t colorH = color >> 8;
    uint8_t colorL = color & 0xFF;

    CLR(SSD1286A_CS);

    ssd1286aSetWindow(x, y, w, h);

    for (uint16_t i = 0; i < w; i++) {
        for (uint16_t j = 0; j < h; j++) {
            ssd1286aSendSPI(colorH);
            ssd1286aSendSPI(colorL);
        }
    }

    while (TX_BUSY());
    SET(SSD1286A_CS);
}

void ssd1286aDrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(SSD1286A_CS);

    ssd1286aSetWindow(x0, y0, w, h);

    DISPDRV_SEND_IMAGE(img, ssd1286aSendData);

    while (TX_BUSY());
    SET(SSD1286A_CS);
}

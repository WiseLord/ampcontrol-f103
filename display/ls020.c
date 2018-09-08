#include "ls020.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_spi.h>

#include "../pins.h"
#include "../functions.h"

#include "gc176x132.h"

#define LS020_WIDTH           132
#define LS020_HEIGHT          176
#define LS020_PIXELS          (LS020_WIDTH * LS020_HEIGHT)

#define TX_BUSY()               (LL_SPI_IsActiveFlag_BSY(SPI1) || !LL_SPI_IsActiveFlag_TXE(SPI1))

static GlcdDriver glcd = {
    .clear = ls020Clear,
    .drawPixel = ls020DrawPixel,
    .drawRectangle = ls020DrawRectangle,
    .drawImage = ls020DrawImage,
};

static inline void ls020SendSPI(uint8_t data) __attribute__((always_inline));
static inline void ls020SendSPI(uint8_t data)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
}

static inline void ls020SendData(uint16_t data) __attribute__((always_inline));
static inline void ls020SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ls020SendSPI(dataH);
    ls020SendSPI(dataL);
}

static void ls020InitSeq(void)
{
    _delay_ms(50);
    SET(LS020_DC);
    CLR(LS020_CS);

    ls020SendData(0xFDFD);
    ls020SendData(0xFDFD);
    _delay_ms(50);
    ls020SendData(0xEF00);
    ls020SendData(0xEE04);
    ls020SendData(0x1B04);
    ls020SendData(0xFEFE);
    ls020SendData(0xFEFE);
    ls020SendData(0xEF90);
    ls020SendData(0x4A04);
    ls020SendData(0x7F3F);
    ls020SendData(0xEE04);
    ls020SendData(0x4306);
    _delay_ms(50);
    ls020SendData(0xEF90);
    ls020SendData(0x0983);
    ls020SendData(0x0800);
    ls020SendData(0x0BAF);
    ls020SendData(0x0A00);
    ls020SendData(0x0500);
    ls020SendData(0x0600);
    ls020SendData(0x0700);
    ls020SendData(0xEF00);
    ls020SendData(0xEE0C);
    ls020SendData(0xEF90);
    ls020SendData(0x0080);
    ls020SendData(0xEFB0);
    ls020SendData(0x4902);
    ls020SendData(0xEF00);
    ls020SendData(0x7F01);
    ls020SendData(0xE181);
    ls020SendData(0xE202);
    ls020SendData(0xE276);
    ls020SendData(0xE183);
    _delay_ms(50);
    ls020SendData(0x8001);
    ls020SendData(0xEF90);
    ls020SendData(0x0020);

    while (TX_BUSY());
    SET(LS020_CS);
}


void ls020SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    SET(LS020_DC);
    CLR(LS020_CS);

    ls020SendData(0x0800 + LS020_WIDTH - y - h);
    ls020SendData(0x0900 + LS020_WIDTH - y - 1);


    ls020SendData(0x0A00 + x);
    ls020SendData(0x0B00 + x + w - 1);

    while (TX_BUSY());
    SET(LS020_CS);
}

static void ls020InitSPI()
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

void ls020Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc176x132Init(*driver);

    // Configure Hardware SPI
    ls020InitSPI();

    CLR(LS020_RST);
    _delay_ms(100);
    SET(LS020_RST);

    // Init magic
    ls020InitSeq();
    SET(LS020_LED);
}

void ls020Clear(void)
{
    ls020DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, glcd.canvas->color);
}

void ls020Sleep(void)
{
    SET(LS020_DC);
    CLR(LS020_CS);
    ls020SendData(0xEF00);
    ls020SendData(0x7E04);
    ls020SendData(0xEFB0);
    ls020SendData(0x5A48);
    ls020SendData(0xEF00);
    ls020SendData(0x7F01);
    ls020SendData(0xEFB0);
    ls020SendData(0x64FF);
    ls020SendData(0x6500);
    ls020SendData(0xEF00);
    ls020SendData(0x7F01);
    ls020SendData(0xE262);
    ls020SendData(0xE202);
    ls020SendData(0xEFB0);
    ls020SendData(0xBC02);
    ls020SendData(0xEF00);
    ls020SendData(0x7F01);
    ls020SendData(0xE200);
    ls020SendData(0x8000);
    ls020SendData(0xE204);
    ls020SendData(0xE200);
    ls020SendData(0xE100);
    ls020SendData(0xEFB0);
    ls020SendData(0xBC00);
    ls020SendData(0xEF00);
    ls020SendData(0x7F01);
    while (TX_BUSY());
    SET(LS020_CS);
}

void ls020Wakeup(void)
{
    ls020InitSeq();
}

void ls020DrawPixel(int16_t x, int16_t y, uint16_t color)
{

    ls020SetWindow(x, y, 1, 1);

    CLR(LS020_DC);
    CLR(LS020_CS);
    ls020SendData(color);
    while (TX_BUSY());
    SET(LS020_CS);
}

void ls020DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t colorH = color >> 8;
    uint8_t colorL = color & 0xFF;

    ls020SetWindow(x, y, w, h);

    CLR(LS020_DC);
    CLR(LS020_CS);

    for (uint16_t i = 0; i < w; i++) {
        for (uint16_t j = 0; j < h; j++) {
            ls020SendSPI(colorH);
            ls020SendSPI(colorL);
        }
    }

    while (TX_BUSY());
    SET(LS020_CS);
}

void ls020DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height / 8;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;
    uint8_t colorH = glcd.font.color >> 8;
    uint8_t colorL = glcd.font.color & 0xFF;
    uint8_t bgColorH = glcd.canvas->color >> 8;
    uint8_t bgColorL = glcd.canvas->color & 0xFF;
    uint8_t mult = glcd.font.mult;

    ls020SetWindow(x0, y0, mult * w, mult * h * 8);

    CLR(LS020_DC);
    CLR(LS020_CS);

    for (uint16_t i = 0; i < w; i++) {
        for (uint8_t mx = 0; mx < mult; mx++) {
            for (uint16_t j = 0; j < h; j++) {
                uint8_t data = img->data[w * j + i];
                for (uint8_t bit = 0; bit < 8; bit++) {
                    if (data & 0x01) {
                        for (uint8_t my = 0; my < mult; my++) {
                            ls020SendSPI(colorH);
                            ls020SendSPI(colorL);
                        }
                    } else {
                        for (uint8_t my = 0; my < mult; my++) {
                            ls020SendSPI(bgColorH);
                            ls020SendSPI(bgColorL);
                        }
                    }
                    data >>= 1;
                }
            }
        }
    }

    while (TX_BUSY());
    SET(LS020_CS);
}

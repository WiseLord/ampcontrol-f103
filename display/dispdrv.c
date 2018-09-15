#include "dispdrv.h"

#include "../pins.h"
#include "../functions.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_spi.h>

#ifdef _DISP_8BIT
static uint8_t bus_requested = 0;
#endif

static GlcdDriver *glcd;

#define TX_BUSY()               (LL_SPI_IsActiveFlag_BSY(SPI1) || !LL_SPI_IsActiveFlag_TXE(SPI1))

#ifdef _DISP_SPI
static void dispdrvInitSPI()
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
#endif

static inline void dispdrvSendByte(uint8_t data) __attribute__((always_inline));
static inline void dispdrvSendByte(uint8_t data)
{
#ifdef _DISP_SPI
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
#else
    DISP_8BIT_DHI_Port->BSRR = 0x00FF0000 | data;    // If port bits 7..0 are used
    CLR(DISP_8BIT_WR);                                // Strob MSB
    SET(DISP_8BIT_WR);
#endif
}

#ifdef _DISP_8BIT
static inline void dispdrvReadInput() __attribute__((always_inline));
static inline void dispdrvReadInput()
{
    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        DISP_8BIT_DHI_Port->BSRR = 0x000000FF;        // Set 1 on all data lines
        DISP_8BIT_DHI_Port->CRL = 0x88888888;         // SET CNF=10, MODE=00 - Input pullup
        // Small delay to stabilize data before reading
        volatile uint8_t delay = 2;
        while (--delay);
        glcd->bus = DISP_8BIT_DHI_Port->IDR & 0x00FF;  // Read 8-bit bus
        DISP_8BIT_DHI_Port->CRL = 0x33333333;         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}
#endif

static inline void dispdrvSendWord(uint16_t data) __attribute__((always_inline));
static inline void dispdrvSendWord(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    dispdrvSendByte(dataH);
    dispdrvSendByte(dataL);

#ifdef _DISP_8BIT
    dispdrvReadInput();
#endif
}

void dispdrvInit(GlcdDriver *driver)
{
    glcd = driver;

#ifdef _DISP_SPI
    dispdrvInitSPI();
#endif
}

void dispdrvBusIRQ(void)
{
#ifdef _DISP_8BIT
    bus_requested = 1;
#endif
}

void dispdrvWaitOperation(void)
{
#ifdef _DISP_SPI
    while (TX_BUSY());
#endif
}


void dispdrvSendData8(uint8_t data)
{
    dispdrvSendByte(data);
#ifdef _DISP_8BIT
    dispdrvReadInput();
#endif
}

void dispdrvSendData16(uint16_t data)
{
    dispdrvSendWord(data);
}

void dispdrvSendFill(uint32_t size, uint16_t color)
{
    for (uint32_t i = 0; i < size; i++) {
        dispdrvSendWord(color);
    }
}

void dispdrvSendImage(tImage *img, uint16_t w, uint16_t h)
{
    uint16_t color = glcd->font.color;
    uint16_t bgColor = glcd->canvas->color;

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
}

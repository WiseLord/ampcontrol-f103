#include "spi.h"

#include "hwlibs.h"

static void pinsInitSpi(SPI_TypeDef *SPIx, bool read)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_TypeDef *gpio = GPIOA;
    uint32_t sckPin = LL_GPIO_PIN_5;
    uint32_t misoPin =  LL_GPIO_PIN_6;
    uint32_t mosiPin = LL_GPIO_PIN_7;

    if (SPIx == SPI2) {
        gpio = GPIOB;
        sckPin = LL_GPIO_PIN_13;
        misoPin = LL_GPIO_PIN_14;
        mosiPin = LL_GPIO_PIN_15;
    }

    GPIO_InitStruct.Pin = sckPin | mosiPin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
#endif
    LL_GPIO_Init(gpio, &GPIO_InitStruct);

    if (read) {
        GPIO_InitStruct.Pin = misoPin;
#ifdef _STM32F1
        GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
#endif
        LL_GPIO_Init(gpio, &GPIO_InitStruct);
    }
}

void spiInit(void *spi, bool read)
{
    SPI_TypeDef *SPIx = (SPI_TypeDef *)spi;

    if (SPIx == SPI1) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
    } else if (SPIx == SPI2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    }

    pinsInitSpi(SPIx, read);

    LL_SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    if (SPIx == SPI1) {
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    } else if (SPIx == SPI2) {
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    }
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
#ifdef _STM32F1
    SPI_InitStruct.CRCPoly = 10;
#endif
#ifdef _STM32F3
    SPI_InitStruct.CRCPoly = 7;
#endif
    LL_SPI_Init(SPIx, &SPI_InitStruct);
#ifdef _STM32F3
    LL_SPI_SetStandard(SPIx, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_DisableNSSPulseMgt(SPIx);
#endif

    LL_SPI_Enable(SPIx);
}

void spiWaitBusy(void *spi)
{
    SPI_TypeDef *SPIx = (SPI_TypeDef *)spi;

    while (LL_SPI_IsActiveFlag_BSY(SPIx) || !LL_SPI_IsActiveFlag_TXE(SPIx));
}

void spiSendByte(void *spi, uint8_t data)
{
    SPI_TypeDef *SPIx = (SPI_TypeDef *)spi;

    while (!LL_SPI_IsActiveFlag_TXE(SPIx));
    LL_SPI_TransmitData8(SPIx, data);

}

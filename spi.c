#include "spi.h"

#include "hwlibs.h"

void spiInit(void *spi)
{
    SPI_TypeDef *SPIx = (SPI_TypeDef *)spi;

    if (SPIx == SPI2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    }

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
    LL_SPI_Init(SPIx, &SPI_InitStruct);

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

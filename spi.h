#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void spiInit(void *spi, bool read);

void spiWaitBusy(void *spi);

void spiSendByte(void *spi, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // SPI_H

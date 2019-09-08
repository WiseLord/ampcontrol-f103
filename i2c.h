#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define I2C_NOACK       0
#define I2C_ACK         1
#define I2C_WRITE       0
#define I2C_READ        1

#define I2C1_BUF_SIZE   160
#define I2C2_BUF_SIZE   0

uint8_t i2cInit(void *i2c, uint32_t ClockSpeed);

void i2cBegin(void *i2c, uint8_t addr);
void i2cSend(void *i2c, uint8_t data);
void i2cTransmit(void *i2c, bool stop);
void i2cReceive(void *i2c, uint8_t *buf, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif // I2C_H

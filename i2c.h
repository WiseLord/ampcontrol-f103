#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <stm32f103xb.h>
#include <stm32f1xx_ll_bus.h>

#define I2C_NOACK       0
#define I2C_ACK         1
#define I2C_WRITE       0
#define I2C_READ        1

#define I2C1_BUF_SIZE   160
#define I2C2_BUF_SIZE   32

uint8_t i2cInit(I2C_TypeDef *I2Cx, uint32_t ClockSpeed);

void i2cBegin(I2C_TypeDef *I2Cx, uint8_t addr);
void i2cSend(I2C_TypeDef *I2Cx, uint8_t data);
void i2cTransmit(I2C_TypeDef *I2Cx, bool stop);
void i2cReceive(I2C_TypeDef *I2Cx, uint8_t *buf, uint8_t size);

#endif // I2C_H

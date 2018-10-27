#ifndef I2C_H
#define I2C_H

#include <inttypes.h>
#include <stm32f103xb.h>
#include <stm32f1xx_ll_bus.h>

#define I2C_NOACK       0
#define I2C_ACK         1
#define I2C_WRITE       0
#define I2C_READ        1

#define I2C_LCD         I2C1
#define I2C_AMP         I2C2
#define I2C_LCD_PERIF   LL_APB1_GRP1_PERIPH_I2C1
#define I2C_AMP_PERIF   LL_APB1_GRP1_PERIPH_I2C2

#define I2C_BUF_SIZE    32

uint8_t i2cInit(I2C_TypeDef *I2Cx, uint32_t ClockSpeed);

void i2cBegin(I2C_TypeDef *I2Cx, uint8_t addr);
void i2cSend(I2C_TypeDef *I2Cx, uint8_t data);
void i2cTransmit(I2C_TypeDef *I2Cx);
void i2cReceive(I2C_TypeDef *I2Cx, uint8_t *buf, uint8_t size);

#endif // I2C_H

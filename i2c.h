#ifndef I2C_H
#define I2C_H

#include <inttypes.h>
#include <stm32f103xb.h>
#include <stm32f1xx_ll_bus.h>

#define I2C_NOACK       0
#define I2C_ACK         1
#define I2C_WRITE       0
#define I2C_READ        1

#define I2C_AMP         I2C1
#define I2C_LCD         I2C2
#define I2C_AMP_PERIF   LL_APB1_GRP1_PERIPH_I2C1
#define I2C_LCD_PERIF   LL_APB1_GRP1_PERIPH_I2C2

uint8_t i2cInit(I2C_TypeDef *I2Cx, uint32_t ClockSpeed);
uint8_t i2cStart(I2C_TypeDef *I2Cx, uint8_t addr);
uint8_t i2cStop(I2C_TypeDef *I2Cx);
uint8_t i2cWrite(I2C_TypeDef *I2Cx, uint8_t data);
uint8_t i2cRead(I2C_TypeDef *I2Cx, uint8_t ack);

uint8_t i2cAmpStart(uint8_t addr);
uint8_t i2cAmpStop(void);
uint8_t i2cAmpWrite(uint8_t data);
uint8_t i2cAmpRead(uint8_t ack);

uint8_t i2cLcdStart(uint8_t addr);
uint8_t i2cLcdStop(void);
uint8_t i2cLcdWrite(uint8_t data);
uint8_t i2cLcdRead(uint8_t ack);

#endif // I2C_H

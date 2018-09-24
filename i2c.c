#include "i2c.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_i2c.h>

uint8_t i2cInit(I2C_TypeDef *I2Cx, uint32_t ClockSpeed)
{
    if (I2Cx == I2C_AMP) {
        LL_APB1_GRP1_EnableClock(I2C_AMP_PERIF);
    } else {
        LL_APB1_GRP1_EnableClock(I2C_LCD_PERIF);
    }

    LL_I2C_Disable(I2Cx);

    LL_I2C_DisableOwnAddress2(I2Cx);
    LL_I2C_DisableGeneralCall(I2Cx);
    LL_I2C_EnableClockStretching(I2Cx);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.ClockSpeed = ClockSpeed;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2Cx, &I2C_InitStruct);

    LL_I2C_SetOwnAddress2(I2Cx, 0);

    LL_I2C_Enable(I2Cx);

    return 0;
}

uint8_t i2cStart(I2C_TypeDef *I2Cx, uint8_t addr)
{
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);

    LL_I2C_GenerateStartCondition(I2Cx);
    while (!LL_I2C_IsActiveFlag_SB(I2Cx));

    LL_I2C_TransmitData8(I2Cx, addr | I2C_WRITE);

    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx));
    LL_I2C_ClearFlag_ADDR(I2Cx);

    return 0;
}

uint8_t i2cStop(I2C_TypeDef *I2Cx)
{
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx));

    LL_I2C_GenerateStopCondition(I2Cx);

    return 0;
}

uint8_t i2cWrite(I2C_TypeDef *I2Cx, uint8_t data)
{
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx));

    LL_I2C_TransmitData8(I2Cx, data);

    return 0;
}

uint8_t i2cRead(I2C_TypeDef *I2Cx, uint8_t ack)
{
    // TODO
    return 0;
}


uint8_t i2cAmpStart(uint8_t addr)
{
    return 0;
    return i2cStart(I2C_AMP, addr);
}

uint8_t i2cAmpStop(void)
{
    return 0;
    return i2cStop(I2C_AMP);
}

uint8_t i2cAmpWrite(uint8_t data)
{
    return 0;
    return i2cWrite(I2C_AMP, data);
}

uint8_t i2cAmpRead(uint8_t ack)
{
    return i2cRead(I2C_AMP, ack);
}


uint8_t i2cLcdStart(uint8_t addr)
{
    return i2cStart(I2C_LCD, addr);
}

uint8_t i2cLcdStop(void)
{
    return i2cStop(I2C_LCD);
}

uint8_t i2cLcdWrite(uint8_t data)
{
    return i2cWrite(I2C_LCD, data);
}

uint8_t i2cLcdRead(uint8_t ack)
{
    return i2cRead(I2C_LCD, ack);
}

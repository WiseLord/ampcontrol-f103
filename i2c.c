#include "i2c.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_i2c.h>

void I2CInit()
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_I2C_Disable(I2C1);

    LL_I2C_DisableOwnAddress2(I2C1);
    LL_I2C_DisableGeneralCall(I2C1);
    LL_I2C_EnableClockStretching(I2C1);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.ClockSpeed = 100000;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);

    LL_I2C_SetOwnAddress2(I2C1, 0);

    LL_I2C_Enable(I2C1);
}

uint8_t I2CStart(uint8_t addr)
{
    return 0;
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);

    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_SB(I2C1));

    LL_I2C_TransmitData8(I2C1, addr | I2C_WRITE);

    while (!LL_I2C_IsActiveFlag_ADDR(I2C1));
    LL_I2C_ClearFlag_ADDR(I2C1);

    return 0;
}

void I2CStop()
{
    return;
    while (!LL_I2C_IsActiveFlag_TXE(I2C1));

    LL_I2C_GenerateStopCondition(I2C1);
}

uint8_t I2CWriteByte(uint8_t data)
{
    return 0;
    while (!LL_I2C_IsActiveFlag_TXE(I2C1));

    LL_I2C_TransmitData8(I2C1, data);

    return 0;
}

uint8_t I2CReadByte(uint8_t ack)
{
    return 0;
}

uint8_t I2CFindDevice(uint8_t addr)
{
    return 0;
}

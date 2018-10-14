#include "i2c.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_i2c.h>

#define I2C_SEND_TIMEOUT_TXE_MS              5
#define I2C_SEND_TIMEOUT_SB_MS               5
#define I2C_SEND_TIMEOUT_ADDR_MS             5

static uint8_t i2cBuf[I2C_BUF_SIZE];
static uint8_t i2cBytes = 0;
static uint8_t i2cAddr = 0;
static uint32_t i2cTimeout = 0;

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

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {

    }

    LL_I2C_TransmitData8(I2Cx, addr | I2C_WRITE);

    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {

    }

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


void i2cBegin(I2C_TypeDef *I2Cx, uint8_t addr)
{
    i2cBytes = 0;
    i2cAddr = addr;
}

void i2cSend(I2C_TypeDef *I2Cx, uint8_t data)
{
    i2cBuf[i2cBytes++] = data;
}

void i2cTransfer(I2C_TypeDef *I2Cx)
{
    // Handle start
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(I2Cx);

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    LL_I2C_TransmitData8(I2Cx, i2cAddr | I2C_WRITE);

    i2cTimeout = I2C_SEND_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    LL_I2C_ClearFlag_ADDR(I2Cx);

    for (uint8_t i = 0; i < i2cBytes; i++) {

        i2cTimeout = I2C_SEND_TIMEOUT_TXE_MS;
        while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (i2cTimeout-- == 0) {
                    // TODO: handle error
                    return;
                }
            }
        }

        LL_I2C_TransmitData8(I2Cx, i2cBuf[i]);
    }

    while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    LL_I2C_GenerateStopCondition(I2Cx);
}

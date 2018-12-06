#include "i2c.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_i2c.h>

#define I2C_SEND_TIMEOUT_TXE_MS              5
#define I2C_SEND_TIMEOUT_RXE_MS              5
#define I2C_SEND_TIMEOUT_SB_MS               5
#define I2C_SEND_TIMEOUT_ADDR_MS             5

static uint8_t i2cBuf[I2C_BUF_SIZE];
static uint8_t i2cBytes = 0;
static uint8_t i2cAddr = 0;
static uint32_t i2cTimeout = 0;

static bool i2cWait(void)
{
    if (LL_SYSTICK_IsActiveCounterFlag()) {
        if (i2cTimeout-- == 0) {
            // TODO: handle error
            return false;
        }
    }
    return true;
}


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

void i2cBegin(I2C_TypeDef *I2Cx, uint8_t addr)
{
    i2cBytes = 0;
    i2cAddr = addr;
}

void i2cSend(I2C_TypeDef *I2Cx, uint8_t data)
{
    i2cBuf[i2cBytes++] = data;
}

void i2cTransmit(I2C_TypeDef *I2Cx, bool stop)
{
    LL_I2C_GenerateStartCondition(I2Cx);

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {
        if (i2cWait() == false)
            return;
    }

    LL_I2C_TransmitData8(I2Cx, i2cAddr | I2C_WRITE);

    i2cTimeout = I2C_SEND_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {
        if (i2cWait() == false)
            return;
    }

    LL_I2C_ClearFlag_ADDR(I2Cx);

    for (uint8_t i = 0; i < i2cBytes; i++) {
        i2cTimeout = I2C_SEND_TIMEOUT_TXE_MS;
        while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {
            if (i2cWait() == false)
                return;
        }
        LL_I2C_TransmitData8(I2Cx, i2cBuf[i]);
    }

    while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {
        if (i2cWait() == false)
            return;
    }

    if (stop) {
        while (!LL_I2C_IsActiveFlag_BTF(I2Cx)) {
            if (i2cWait() == false)
                return;
        }
        LL_I2C_GenerateStopCondition(I2Cx);
    }
}

void i2cReceive(I2C_TypeDef *I2Cx, uint8_t *buf, uint8_t size)
{
    if (size == 2) {
        LL_I2C_EnableBitPOS(I2Cx);
    } else {
        LL_I2C_DisableBitPOS(I2Cx);
    }

    LL_I2C_GenerateStartCondition(I2Cx);

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {
        if (i2cWait() == false)
            return;
    }

    LL_I2C_TransmitData8(I2Cx, i2cAddr | I2C_READ);

    i2cTimeout = I2C_SEND_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {
        if (i2cWait() == false)
            return;
    }

    if (size == 1) {
        LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
        LL_I2C_ClearFlag_ADDR(I2Cx);
        LL_I2C_GenerateStopCondition(I2Cx);

        i2cTimeout = I2C_SEND_TIMEOUT_RXE_MS;
        while (!LL_I2C_IsActiveFlag_RXNE(I2Cx)) {
            if (i2cWait() == false)
                return;
        }
        *buf++ = LL_I2C_ReceiveData8(I2Cx);
    } else if (size == 2) {
        LL_I2C_ClearFlag_ADDR(I2Cx);
        LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);

        while (!LL_I2C_IsActiveFlag_BTF(I2Cx)) {
            if (i2cWait() == false)
                return;
        }

        LL_I2C_GenerateStopCondition(I2Cx);

        *buf++ = LL_I2C_ReceiveData8(I2Cx);
        *buf++ = LL_I2C_ReceiveData8(I2Cx);
    } else {
        LL_I2C_ClearFlag_ADDR(I2Cx);

        while (size) {
            /* Receive bytes from first byte until byte N-3 */
            if (size-- != 3) {
                while (!LL_I2C_IsActiveFlag_BTF(I2Cx)) {
                    if (i2cWait() == false)
                        return;
                }

                *buf++ = LL_I2C_ReceiveData8(I2Cx);
            } else {
                while (!LL_I2C_IsActiveFlag_BTF(I2Cx)) {
                    if (i2cWait() == false)
                        return;
                }

                LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);

                *buf++ = LL_I2C_ReceiveData8(I2Cx);

                LL_I2C_GenerateStopCondition(I2Cx);

                *buf++ = LL_I2C_ReceiveData8(I2Cx);

                i2cTimeout = I2C_SEND_TIMEOUT_RXE_MS;
                while (!LL_I2C_IsActiveFlag_RXNE(I2Cx)) {
                    if (i2cWait() == false)
                        return;
                }
                *buf++ = LL_I2C_ReceiveData8(I2Cx);

                size = 0;
            }
        }
    }

    while (LL_I2C_IsActiveFlag_STOP(I2Cx)) {
        if (i2cWait() == false)
            return;
    }
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
    LL_I2C_DisableBitPOS(I2Cx);
}

#include "i2c.h"

#include <stddef.h>

#include "hwlibs.h"

#define I2C_TIMEOUT_TXE_MS          5
#define I2C_TIMEOUT_RXE_MS          5
#define I2C_TIMEOUT_SB_MS           5
#define I2C_TIMEOUT_STOP_MS         5
#define I2C_TIMEOUT_ADDR_MS         5
#define I2C_TIMEOUT_BTF_MS          5
#define I2C_SEND_TIMEOUT_TXIS_MS    5

typedef struct {
    uint32_t pefBit;
    uint8_t *buf;
    uint8_t bytes;
    uint8_t addr;
    uint8_t timeout;
} I2cContext;

#if I2C1_BUF_SIZE
static uint8_t i2c1Buf[I2C1_BUF_SIZE];
static I2cContext i2cCtx1 = {LL_APB1_GRP1_PERIPH_I2C1, i2c1Buf, 0, 0, 0};
#endif

#if I2C2_BUF_SIZE
static uint8_t i2c2Buf[I2C2_BUF_SIZE];
static I2cContext i2cCtx2 = {LL_APB1_GRP1_PERIPH_I2C2, i2c2Buf, 0, 0, 0};
#endif

static I2cContext *getI2cCtx(I2C_TypeDef *I2Cx)
{
    if (I2Cx == I2C1) {
#if I2C1_BUF_SIZE
        return &i2cCtx1;
#else
        return NULL;
#endif
    } else if (I2Cx == I2C2) {
#if I2C2_BUF_SIZE
        return &i2cCtx2;
#else
        return NULL;
#endif
    } else {
        return NULL;
    }
}

static bool i2cWait(I2cContext *ctx)
{
    if (LL_SYSTICK_IsActiveCounterFlag()) {
        if (ctx->timeout-- == 0) {
            // TODO: Handle errors
            return false;
        }
    }
    return true;
}

uint8_t i2cInit(void *i2c, uint32_t ClockSpeed)
{
    I2cContext *ctx = getI2cCtx(i2c);

    if (ctx == NULL)
        return 1;

    LL_APB1_GRP1_EnableClock(ctx->pefBit);

#ifdef _STM32F3
    LL_I2C_EnableAutoEndMode(i2c);
#endif
    LL_I2C_DisableOwnAddress2(i2c);
    LL_I2C_DisableGeneralCall(i2c);
    LL_I2C_EnableClockStretching(i2c);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
#ifdef _STM32F1
    I2C_InitStruct.ClockSpeed = ClockSpeed;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
#endif
#ifdef _STM32F3
    I2C_InitStruct.Timing = 0x10808DD3; // TODO: calculate from ClockSpeed
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
#endif
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(i2c, &I2C_InitStruct);

#ifdef _STM32F3
    LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
#endif
    return 0;
}

void i2cBegin(void *i2c, uint8_t addr)
{
    I2cContext *ctx = getI2cCtx(i2c);
    if (ctx == NULL)
        return;

    ctx->bytes = 0;
    ctx->addr = addr;
}

void i2cSend(void *i2c, uint8_t data)
{
    I2cContext *ctx = getI2cCtx(i2c);
    if (ctx == NULL)
        return;

    ctx->buf[ctx->bytes++] = data;
}


void i2cTransmit(void *i2c, bool stop)
{
    I2cContext *ctx = getI2cCtx(i2c);
    if (ctx == NULL)
        return;

#ifdef _STM32F1
    LL_I2C_GenerateStartCondition(i2c);

    ctx->timeout = I2C_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(i2c)) {
        if (i2cWait(ctx) == false)
            return;
    }

    LL_I2C_TransmitData8(i2c, ctx->addr | I2C_WRITE);

    ctx->timeout = I2C_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(i2c)) {
        if (i2cWait(ctx) == false) {
            LL_I2C_GenerateStopCondition(i2c);
            return;
        }
    }

    LL_I2C_ClearFlag_ADDR(i2c);

    for (uint8_t i = 0; i < ctx->bytes; i++) {
        ctx->timeout = I2C_TIMEOUT_TXE_MS;
        while (!LL_I2C_IsActiveFlag_TXE(i2c)) {
            if (i2cWait(ctx) == false)
                return;
        }
        LL_I2C_TransmitData8(i2c, ctx->buf[i]);
    }

    if (stop) {
        ctx->timeout = I2C_TIMEOUT_BTF_MS;
        while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
            if (i2cWait(ctx) == false)
                return;
        }
        LL_I2C_GenerateStopCondition(i2c);
    }
#endif

#ifdef _STM32F3
    LL_I2C_HandleTransfer(i2c, ctx->addr, LL_I2C_ADDRSLAVE_7BIT, ctx->bytes,
                          LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    uint8_t *pBuf    = (uint8_t *)ctx->buf;

    while (!LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (LL_I2C_IsActiveFlag_TXIS(i2c)) {
            LL_I2C_TransmitData8(i2c, (*pBuf++));

            ctx->timeout = I2C_SEND_TIMEOUT_TXIS_MS;
        }

        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cWait(ctx) == false)
                return;
        }
    }

//    if (stop) {
//        ctx->timeout = I2C_TIMEOUT_BTF_MS;
//        while (!LL_I2C_IsActiveFlag_TC(i2c)) {
//            if (i2cWait(ctx) == false)
//                return;
//        }
//        LL_I2C_GenerateStopCondition(i2c);
//    }

    LL_I2C_ClearFlag_STOP(I2C1);
#endif
}

void i2cReceive(void *i2c, uint8_t *buf, uint8_t size)
{
    I2cContext *ctx = getI2cCtx(i2c);
    if (ctx == NULL)
        return;

#ifdef _STM32F1
    if (size == 2) {
        LL_I2C_EnableBitPOS(i2c);
    } else {
        LL_I2C_DisableBitPOS(i2c);
    }

    LL_I2C_GenerateStartCondition(i2c);

    ctx->timeout = I2C_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(i2c)) {
        if (i2cWait(ctx) == false)
            return;
    }

    LL_I2C_TransmitData8(i2c, ctx->addr | I2C_READ);

    ctx->timeout = I2C_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(i2c)) {
        if (i2cWait(ctx) == false) {
            LL_I2C_GenerateStopCondition(i2c);
            return;
        }
    }

    if (size == 1) {
        LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
        LL_I2C_ClearFlag_ADDR(i2c);
        LL_I2C_GenerateStopCondition(i2c);

        ctx->timeout = I2C_TIMEOUT_RXE_MS;
        while (!LL_I2C_IsActiveFlag_RXNE(i2c)) {
            if (i2cWait(ctx) == false)
                return;
        }
        *buf++ = LL_I2C_ReceiveData8(i2c);
    } else if (size == 2) {
        LL_I2C_ClearFlag_ADDR(i2c);
        LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);

        ctx->timeout = I2C_TIMEOUT_BTF_MS;
        while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
            if (i2cWait(ctx) == false)
                return;
        }

        LL_I2C_GenerateStopCondition(i2c);

        *buf++ = LL_I2C_ReceiveData8(i2c);
        *buf++ = LL_I2C_ReceiveData8(i2c);
    } else {
        LL_I2C_ClearFlag_ADDR(i2c);

        while (size) {
            /* Receive bytes from first byte until byte N-3 */
            if (size-- != 3) {
                ctx->timeout = I2C_TIMEOUT_BTF_MS;
                while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
                    if (i2cWait(ctx) == false)
                        return;
                }

                *buf++ = LL_I2C_ReceiveData8(i2c);
            } else {
                ctx->timeout = I2C_TIMEOUT_BTF_MS;
                while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
                    if (i2cWait(ctx) == false)
                        return;
                }

                LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);

                *buf++ = LL_I2C_ReceiveData8(i2c);

                LL_I2C_GenerateStopCondition(i2c);

                *buf++ = LL_I2C_ReceiveData8(i2c);

                ctx->timeout = I2C_TIMEOUT_RXE_MS;
                while (!LL_I2C_IsActiveFlag_RXNE(i2c)) {
                    if (i2cWait(ctx) == false)
                        return;
                }
                *buf++ = LL_I2C_ReceiveData8(i2c);

                size = 0;
            }
        }
    }

    ctx->timeout = I2C_TIMEOUT_STOP_MS;
    while (LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (i2cWait(ctx) == false)
            return;
    }
    LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
    LL_I2C_DisableBitPOS(i2c);
#endif

}

#include "i2c.h"

#include <stddef.h>

#include "hwlibs.h"

#define I2C_TIMEOUT_MS      3

typedef struct {
    uint8_t *buf;
    uint8_t bytes;
    uint8_t addr;
    uint8_t timeout;
} I2cContext;

#if I2C1_BUF_SIZE
static uint8_t i2c1Buf[I2C1_BUF_SIZE];
static I2cContext i2cCtx1 = {i2c1Buf, 0, 0, 0};
#endif

#if I2C2_BUF_SIZE
static uint8_t i2c2Buf[I2C2_BUF_SIZE];
static I2cContext i2cCtx2 = {i2c2Buf, 0, 0, 0};
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

static void i2cInitPins(I2C_TypeDef *I2Cx)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_TypeDef *gpio = GPIOB;
    uint32_t sclPin =  LL_GPIO_PIN_8;   // LL_GPIO_PIN_6
    uint32_t sdaPin =  LL_GPIO_PIN_9;   // LL_GPIO_PIN_7

    if (I2Cx == I2C1) {
        if (sclPin == LL_GPIO_PIN_8 && sdaPin == LL_GPIO_PIN_9) {
#ifdef STM32F1
            LL_GPIO_AF_EnableRemap_I2C1();
#endif
        }
    } else if (I2Cx == I2C2) {
        sclPin = LL_GPIO_PIN_10;
        sdaPin = LL_GPIO_PIN_11;
    }

    GPIO_InitStruct.Pin = sclPin | sdaPin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
#ifdef STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
#endif

    LL_GPIO_Init(gpio, &GPIO_InitStruct);
}

#ifdef STM32F1
static void i2cDoStop(I2C_TypeDef *I2Cx)
{
    I2cContext *ctx = getI2cCtx(I2Cx);

#ifdef STM32F1
    LL_I2C_GenerateStopCondition(I2Cx);
    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_STOP(I2Cx)) {
        if (i2cWait(ctx) == false)
            return;
    }
#endif
}
#endif

uint8_t i2cInit(void *i2c, uint32_t ClockSpeed)
{
    I2cContext *ctx = getI2cCtx(i2c);

    if (ctx == NULL)
        return 1;

    I2C_TypeDef *I2Cx = (I2C_TypeDef *)i2c;

    if (I2Cx == I2C1) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
    } else if (I2Cx == I2C2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
    }

    i2cInitPins(I2Cx);

#ifdef STM32F3
    LL_I2C_EnableAutoEndMode(i2c);
#endif
    LL_I2C_DisableOwnAddress2(i2c);
    LL_I2C_DisableGeneralCall(i2c);
    LL_I2C_EnableClockStretching(i2c);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
#ifdef STM32F1
    I2C_InitStruct.ClockSpeed = ClockSpeed;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
#endif
#ifdef STM32F3
    I2C_InitStruct.Timing = 0x2000090E; // TODO: calculate from ClockSpeed
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
#endif
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(i2c, &I2C_InitStruct);

#ifdef STM32F3
    LL_I2C_SetOwnAddress2(i2c, 0, LL_I2C_OWNADDRESS2_NOMASK);
#endif
    return 0;
}

uint8_t i2cDeInit(void *i2c)
{
    LL_I2C_DeInit(i2c);

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


void i2cTransmit(void *i2c)
{
    I2cContext *ctx = getI2cCtx(i2c);
    if (ctx == NULL)
        return;

#ifdef STM32F1
    LL_I2C_GenerateStartCondition(i2c);

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_SB(i2c)) {
        if (i2cWait(ctx) == false) {
            i2cDoStop(i2c);
            return;
        }
    }

    LL_I2C_TransmitData8(i2c, ctx->addr | I2C_WRITE);

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(i2c)) {
        if (i2cWait(ctx) == false) {
            i2cDoStop(i2c);
            return;
        }
    }

    LL_I2C_ClearFlag_ADDR(i2c);

    for (uint8_t i = 0; i < ctx->bytes; i++) {
        ctx->timeout = I2C_TIMEOUT_MS;
        while (!LL_I2C_IsActiveFlag_TXE(i2c)) {
            if (i2cWait(ctx) == false) {
                i2cDoStop(i2c);
                return;
            }
        }
        LL_I2C_TransmitData8(i2c, ctx->buf[i]);
    }

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
        if (i2cWait(ctx) == false) {
            i2cDoStop(i2c);
            return;
        }
    }
    i2cDoStop(i2c);
#endif

#ifdef STM32F3
    LL_I2C_HandleTransfer(i2c, ctx->addr, LL_I2C_ADDRSLAVE_7BIT, ctx->bytes,
                          LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    uint8_t *pBuf    = (uint8_t *)ctx->buf;

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (LL_I2C_IsActiveFlag_TXIS(i2c)) {
            LL_I2C_TransmitData8(i2c, (*pBuf++));
            ctx->timeout = I2C_TIMEOUT_MS;
        }
        if (i2cWait(ctx) == false) {
            LL_I2C_ClearFlag_STOP(i2c);
            return;
        }
    }

    LL_I2C_ClearFlag_STOP(i2c);
#endif
}

void i2cReceive(void *i2c, uint8_t *buf, uint8_t size)
{
    I2cContext *ctx = getI2cCtx(i2c);
    if (ctx == NULL)
        return;

#ifdef STM32F1
    if (size == 2) {
        LL_I2C_EnableBitPOS(i2c);
    } else {
        LL_I2C_DisableBitPOS(i2c);
    }

    LL_I2C_GenerateStartCondition(i2c);

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_SB(i2c)) {
        if (i2cWait(ctx) == false)
            return;
    }

    LL_I2C_TransmitData8(i2c, ctx->addr | I2C_READ);

    ctx->timeout = I2C_TIMEOUT_MS;
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

        ctx->timeout = I2C_TIMEOUT_MS;
        while (!LL_I2C_IsActiveFlag_RXNE(i2c)) {
            if (i2cWait(ctx) == false)
                return;
        }
        *buf++ = LL_I2C_ReceiveData8(i2c);
    } else if (size == 2) {
        LL_I2C_ClearFlag_ADDR(i2c);
        LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);

        ctx->timeout = I2C_TIMEOUT_MS;
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
                ctx->timeout = I2C_TIMEOUT_MS;
                while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
                    if (i2cWait(ctx) == false)
                        return;
                }

                *buf++ = LL_I2C_ReceiveData8(i2c);
            } else {
                ctx->timeout = I2C_TIMEOUT_MS;
                while (!LL_I2C_IsActiveFlag_BTF(i2c)) {
                    if (i2cWait(ctx) == false)
                        return;
                }

                LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);

                *buf++ = LL_I2C_ReceiveData8(i2c);

                LL_I2C_GenerateStopCondition(i2c);

                *buf++ = LL_I2C_ReceiveData8(i2c);

                ctx->timeout = I2C_TIMEOUT_MS;
                while (!LL_I2C_IsActiveFlag_RXNE(i2c)) {
                    if (i2cWait(ctx) == false)
                        return;
                }
                *buf++ = LL_I2C_ReceiveData8(i2c);

                size = 0;
            }
        }
    }

    ctx->timeout = I2C_TIMEOUT_MS;
    while (LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (i2cWait(ctx) == false) {
            return;
        }
    }
    LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
    LL_I2C_DisableBitPOS(i2c);
#endif

#ifdef STM32F3
    LL_I2C_HandleTransfer(i2c, ctx->addr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (LL_I2C_IsActiveFlag_RXNE(i2c)) {
            *buf++ = LL_I2C_ReceiveData8(i2c);
            ctx->timeout = I2C_TIMEOUT_MS;
        }
        if (i2cWait(ctx) == false)
            return;
    }

    LL_I2C_ClearFlag_STOP(i2c);
#endif
}

#include "demod.h"

#include <stdbool.h>

#include "decoder.h"
#include "hwlibs.h"
#include "ringbuf.h"

#define RING_BUF_SIZE 128

static RingBuf rbuf;
static char rbData[RING_BUF_SIZE];

static void rdsDemodInitPins(void)
{
#ifdef STM32F1
    LL_GPIO_AF_SetEXTISource(RDCL_AR_ExtiPort, RDCL_AR_ExtiLine);
    LL_GPIO_SetPinMode(RDCL_Port, RDCL_Pin, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(RDDA_Port, RDDA_Pin, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(QUAL_Port, QUAL_Pin, LL_GPIO_MODE_FLOATING);
#endif

    LL_EXTI_DisableEvent_0_31(RDCL_ExtiLine);
    LL_EXTI_EnableIT_0_31(RDCL_ExtiLine);
    LL_EXTI_EnableFallingTrig_0_31(RDCL_ExtiLine);
}

static void rdsDemodIRQ(void)
{
    bool rdda = !!READ(RDDA);

    ringBufPushChar(&rbuf, rdda);
}

void rdsDemodInit(void)
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));

    rdsDemodInitPins();

    NVIC_SetPriority(EXTI2_IRQn, 0);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void rdsDemodHandle(void)
{
    uint16_t size = ringBufGetSize(&rbuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&rbuf);
        rdsDecoderPushBit(ch);
    }
}

void EXTI_RDS_HANDLER()
{
    if (LL_EXTI_IsActiveFlag_0_31(RDCL_ExtiLine) != RESET) {
        // Clear RC line interrupt
        LL_EXTI_ClearFlag_0_31(RDCL_ExtiLine);

        // Callback
        rdsDemodIRQ();
    }
}


#include "debug.h"

#include "usart.h"

#include <stm32f1xx_ll_usart.h>

static void dbgPutString(const char *str)
{
    while (*str) {
        usartSendChar(USART_DBG, *str++);
    }
}

void dbgInit()
{
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART1_IRQn);

    usartInit(USART_DBG, 115200);
    LL_USART_EnableIT_RXNE(USART_DBG);
    usartSendChar(USART_DBG, '\r');
}

char dbgGetChar()
{
    return LL_USART_ReceiveData8(USART_DBG);
}

void dbg(const char *str)
{
    dbgPutString(str);
    dbgPutString("\r\n");
}

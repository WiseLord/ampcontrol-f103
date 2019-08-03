#include "debug.h"

#include "mem.h"
#include "usart.h"

#include <stm32f1xx_ll_usart.h>
#include <stdarg.h>
#include <stdio.h>

__attribute__((always_inline))
static inline void dbgPutChar(char ch)
{
    usartSendChar(USART_DBG, ch);
}

static void dbgPutString(char *str)
{
    while (*str) {
        dbgPutChar(*str++);
    }
}

void dbgInit()
{
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART1_IRQn);

    usartInit(USART_DBG, 115200);
    LL_USART_EnableIT_RXNE(USART_DBG);
}

void dbgSendChar(char ch)
{
    dbgPutChar(ch);
}

void dbgPrintf (const char *fmt, ...)
{
    char *buffer = mem_malloc(2048);

    va_list args;
    va_start (args, fmt);
    vsnprintf (buffer, 2048, fmt, args);
    va_end (args);

    dbgPutString(buffer);

    mem_free(buffer);
}

void dbgIRQ()
{
    char data = LL_USART_ReceiveData8(USART_DBG);

#ifdef _DEBUG_KARADIO
    usartSendChar(USART_KARADIO, data);
#else
    (void)data;
#endif
}

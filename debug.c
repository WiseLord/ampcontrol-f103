#include "debug.h"

#include "mem.h"
#include "usart.h"

#include <stdarg.h>
#include <stdio.h>

#define USART_DBG               USART2

static void dbgPutChar(char ch)
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
    usartInit(USART_DBG, 115200);
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

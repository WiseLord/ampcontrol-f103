#include "debug.h"

#include "hwlibs.h"
#include "usart.h"

static void dbgPutString(const char *str)
{
    while (*str) {
        usartSendChar(USART_DBG, *str++);
    }
}

void dbgInit()
{
    usartInit(USART_DBG, 115200);
    usartSetRxIRQ(USART_DBG, true);

    usartSendChar(USART_DBG, '\r');
}

void dbg(const char *str)
{
    dbgPutString(str);
    dbgPutString("\r\n");
}

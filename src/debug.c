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

#ifdef _PRINTF_USART1
int _write(int fd, char* str, int len)
{
    (void)fd;
    int i = 0;
    while (str[i] && (i < len)) {
        usartSendChar(USART1, (int)str[i]);
        if (str[i] == '\n') {
            usartSendChar(USART1, (int)'\r');
        }
        i++;
    }
    return len;
}
#endif

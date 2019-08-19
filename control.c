#include "control.h"

#include "debug.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"

#include <stm32f1xx_ll_usart.h>
#include <stdbool.h>

#define CMDBUF_SIZE     64

static RingBuf cmdRb;
static char cmdRbData[CMDBUF_SIZE];
static LineParse cmdLp;

static void controlParseLine(char *line)
{
    dbg(utilMkStr("Parse '%s'", line));
}

void controlInit(void)
{
    ringBufInit(&cmdRb, cmdRbData, sizeof(cmdRbData));
}

void controlIRQ(void)
{
    char data = LL_USART_ReceiveData8(USART_DBG);

#ifdef _DEBUG_KARADIO
    usartSendChar(USART_KARADIO, data);
#endif

    ringBufPushChar(&cmdRb, data);
}

void controlGetData(void)
{
    uint16_t size = ringBufGetSize(&cmdRb);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&cmdRb);
        if (utilReadChar(&cmdLp, ch)) {
            controlParseLine(cmdLp.line);
        }
    }
}

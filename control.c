#include "control.h"

#include "debug.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"
#include <stm32f1xx_ll_usart.h>

#define CMDBUF_SIZE     64

static RingBuf cmdBuf;
static char cmdBufData[CMDBUF_SIZE];

static int32_t bufIdx = 0;
static char lineBuf[CMDBUF_SIZE];

static void controlParseLine(char *line)
{
    dbg(utilMkStr("Parse '%s'", line));
}

void controlInit(void)
{
    ringBufInit(&cmdBuf, cmdBufData, sizeof (cmdBufData));
}

void controlIRQ(void)
{
    char data = LL_USART_ReceiveData8(USART_DBG);

#ifdef _DEBUG_KARADIO
    usartSendChar(USART_KARADIO, data);
#endif

    ringBufPushChar(&cmdBuf, data);
}

static void controlRead(char data)
{
    switch (data) {
    case '\n':
    case '\r':
        if (bufIdx == 0) {
            break;
        }
        lineBuf[bufIdx] = 0;
        controlParseLine(lineBuf);
        bufIdx = 0;
        break;
    default:
        lineBuf[bufIdx++] = data;
        if (bufIdx >= CMDBUF_SIZE) {
            lineBuf[--bufIdx] = 0;
        }
        break;
    }
}

void controlGetData(void)
{
    uint16_t size = ringBufGetSize(&cmdBuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&cmdBuf);
        controlRead(ch);
    }
}

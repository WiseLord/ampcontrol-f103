#include "karadio.h"

#include <stm32f1xx_ll_usart.h>
#include <string.h>

#include "usart.h"
#include "usb/hidkeys.h"

#include "debug.h"

#define CMD_CLI     "cli"
#define CMD_SYS     "sys"
#define CMD_WIFI    "wifi"

#define CLI_PLAY    "start"
#define CLI_STOP    "stop"
#define CLI_PREV    "prev"
#define CLI_NEXT    "next"
#define CLI_INFO  "info"

#define CLI_META    "##CLI.META#:"
#define CLI_NAMESET "##CLI.NAMESET#:"
#define CLI_STOPPED "##CLI.STOPPED#"
#define CLI_PLAYING "##CLI.PLAYING#"

#define RX_BUF_SIZE     128
#define ST_NAME_SIZE    40
#define ST_META_SIZE    64

static char rxBuf[RX_BUF_SIZE];
static char stName[ST_NAME_SIZE];
static char stMeta[ST_META_SIZE];

static int32_t bufIdx = 0;

static KaRadioData krData;

static void karadioPutChar(char ch)
{
    usartSendChar(USART_KARADIO, ch);
}

static void karadioSendCmd (const char *type, const char *cmd)
{
    const char *str;
    karadioPutChar('\n');

    str = type;
    while (*str) {
        karadioPutChar(*str++);
    }
    karadioPutChar('.');

    str = cmd;
    while (*str) {
        karadioPutChar(*str++);
    }

    karadioPutChar('\n');
}

void karadioInit(void)
{
    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART2_IRQn);

    usartInit(USART_KARADIO, 115200);
    LL_USART_EnableIT_RXNE(USART_KARADIO);

    krData.name = stName;
    krData.meta = stMeta;

    krData.flags = 0;

    bufIdx = 0;
}

KaRadioData *karadioGet(void)
{
    return &krData;
}

void karadioUpdateStatus(void)
{
    karadioSendCmd(CMD_CLI, CLI_INFO);
}

void karadioSendMediaCmd(uint8_t cmd)
{
    switch (cmd) {
    case HIDMEDIAKEY_STOP:
        karadioSendCmd(CMD_CLI, CLI_STOP);
        break;
    case HIDMEDIAKEY_PLAY:
        karadioSendCmd(CMD_CLI, CLI_PLAY);
        break;
    case HIDMEDIAKEY_PREV_TRACK:
        karadioSendCmd(CMD_CLI, CLI_PREV);
        break;
    case HIDMEDIAKEY_NEXT_TRACK:
        karadioSendCmd(CMD_CLI, CLI_NEXT);
        break;
    default:
        break;
    }
}

static void parse(char *line)
{
    char *ici;

    if ((ici = strstr(line, CLI_PLAYING)) != NULL) {
        krData.playing = true;
        krData.flags |= (KARADIO_FLAG_ALL);
    } else  if ((ici = strstr(line, CLI_STOPPED)) != NULL) {
        krData.playing = false;
        krData.flags |= (KARADIO_FLAG_ALL);
    } else  if ((ici = strstr(line, CLI_META)) != NULL) {
        strncpy(krData.meta, ici + sizeof(CLI_META), ST_META_SIZE);
        krData.flags |= KARADIO_FLAG_META;
    } else if ((ici = strstr(line, CLI_NAMESET)) != NULL) {
        strncpy(krData.name, ici + sizeof(CLI_NAMESET), ST_NAME_SIZE);
        krData.flags |= KARADIO_FLAG_NAME;
    }
}

void karadioIRQ()
{
    char data;

    data = LL_USART_ReceiveData8(USART_KARADIO);

    dbgSendChar(data);

    switch (data) {
    case '\n':
    case '\r':
        if (bufIdx == 0) {
            break;
        }
        rxBuf[bufIdx] = 0;
        parse(rxBuf);
        bufIdx = 0;
        break;
    default:
        rxBuf[bufIdx] = data;
        if (++bufIdx >= RX_BUF_SIZE) {
            bufIdx = 0;
            rxBuf[bufIdx] = 0;
        }
        break;
    }
}

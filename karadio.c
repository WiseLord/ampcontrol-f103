#include "karadio.h"

#include <stm32f1xx_ll_usart.h>
#include <string.h>

#include "audio/audio.h"
#include "ringbuf.h"
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
#define CLI_INFO    "info"

#define CLI_META    "##CLI.META#:"
#define CLI_NAMESET "##CLI.NAMESET#:"
#define CLI_STOPPED "##CLI.STOPPED#"
#define CLI_PLAYING "##CLI.PLAYING#"

#define TRYING      "Trying "
#define IP          "ip:"
#define AUTOSTART   "autostart:"

#define SYS_BOOT    "boot"
#define WIFI_DISCON "discon"

#define LINE_SIZE       128
#define ST_NAME_SIZE    40
#define ST_META_SIZE    64
#define RINGBUF_SIZE    512

static char lineBuf[LINE_SIZE];
static char stName[ST_NAME_SIZE];
static char stMeta[ST_META_SIZE];

static int32_t bufIdx = 0;

static KaRadioData krData;
static bool kEnabled = true;

static RingBuf ringBuf;
static char ringBufData[RINGBUF_SIZE];

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

static void karadioClearStatus()
{
    krData.flags = 0;
    bufIdx = 0;
    krData.playing = false;

    memset(stName, 0, ST_NAME_SIZE);
    memset(stMeta, 0, ST_META_SIZE);
}

void karadioInit(void)
{
    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART2_IRQn);

    krData.name = stName;
    krData.meta = stMeta;

    karadioClearStatus();

    ringBufInit(&ringBuf, ringBufData, sizeof (ringBufData));

    usartInit(USART_KARADIO, 115200);
    LL_USART_EnableIT_RXNE(USART_KARADIO);
}

void karadioSetEnabled(bool value)
{
    if (value && !kEnabled) {
//        karadioUpdateStatus();
        karadioSendCmd(CMD_SYS, SYS_BOOT);
    } else if (!value && kEnabled) {
        karadioSendCmd(CMD_WIFI, WIFI_DISCON);
        karadioClearStatus();
    }

    kEnabled = value;
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

static void karadioParseLine(char *line)
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

        AudioProc *aProc = audioGet();
        InputType inType = aProc->par.inType[aProc->par.input];
        karadioSetEnabled(inType == IN_KARADIO);
    } else if ((ici = strstr(line, CLI_NAMESET)) != NULL) {
        strncpy(krData.name, ici + sizeof(CLI_NAMESET), ST_NAME_SIZE);
        krData.flags |= KARADIO_FLAG_NAME;
    } else if ((ici = strstr(line, TRYING)) != NULL) {
        krData.playing = true;
        strncpy(krData.meta, ici, ST_META_SIZE);
        krData.flags |= KARADIO_FLAG_META;
    } else if ((ici = strstr(line, IP)) != NULL) {
        krData.playing = true;
        strncpy(krData.meta, ici, ST_META_SIZE);
        krData.flags |= KARADIO_FLAG_META;
    }
}

static void karadioRead(char data)
{
    switch (data) {
    case '\n':
    case '\r':
        if (bufIdx == 0) {
            break;
        }
        lineBuf[bufIdx] = 0;
        karadioParseLine(lineBuf);
        bufIdx = 0;
        break;
    default:
        lineBuf[bufIdx] = data;
        if (++bufIdx >= LINE_SIZE) {
            bufIdx = 0;
            lineBuf[bufIdx] = 0;
        }
        break;
    }
}

void karadioGetData(void)
{
    uint16_t size = ringBufGetSize(&ringBuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&ringBuf);
        karadioRead(ch);
    }
}

void karadioIRQ()
{
    char data = LL_USART_ReceiveData8(USART_KARADIO);

#ifdef _DEBUG_KARADIO
    dbgSendChar(data);
#endif

    ringBufPushChar(&ringBuf, data);
}

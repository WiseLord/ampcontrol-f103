#include "karadio.h"

#include <string.h>

#include "amp.h"
#include "audio/audio.h"
#include "debug.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"

#define KARADIO_RB_SIZE 128

static KaRadioData krData;

static RingBuf krRingBuf;
static char krRbData[KARADIO_RB_SIZE];
static LineParse krLp;

static void karadioSendCmdCli(const char *cmd)
{
    usartSendChar(USART_KARADIO, '\n');
    usartSendString(USART_KARADIO, "cli");
    usartSendChar(USART_KARADIO, '.');
    usartSendString(USART_KARADIO, cmd);
    usartSendChar(USART_KARADIO, '\n');
}

static void karadioClearStatus()
{
    krLp.idx = 0;

    memset(&krData, 0, sizeof(KaRadioData));
}

void karadioInit(void)
{
    karadioClearStatus();

    ringBufInit(&krRingBuf, krRbData, sizeof(krRbData));

    usartInit(USART_KARADIO, 115200);
    usartSetRxIRQ(USART_KARADIO, true);
}

void karadioSetEnabled(bool value)
{
    if (value) {
        karadioSendCmdCli("start");
    } else {
        karadioSendCmdCli("stop");
        karadioClearStatus();
    }
}

KaRadioData *karadioGet(void)
{
    return &krData;
}

void karadioUpdateStatus(void)
{
    karadioSendCmdCli("info");
}

void karadioSendMediaKey(HidMediaKey cmd)
{
    switch (cmd) {
    case HIDMEDIAKEY_STOP:
        karadioSendCmdCli("stop");
        break;
    case HIDMEDIAKEY_PLAY:
        karadioSendCmdCli("start");
        break;
    case HIDMEDIAKEY_PREV_TRACK:
        karadioSendCmdCli("prev");
        break;
    case HIDMEDIAKEY_NEXT_TRACK:
        karadioSendCmdCli("next");
        break;
    default:
        break;
    }
}

static void karadioUpdateNumber(const char *str, size_t numLen)
{
    if (numLen > ST_NUM_SIZE)
        numLen = ST_NUM_SIZE;
    strncpy(krData.num, str, numLen);
    krData.num[numLen] = '\0';
}

static void trimEnd(char *string)
{
    size_t len = strlen(string);

    // Remove possible service symbols and spaces at the end
    while (len > 0 && (string[len - 1] <= ' ')) {
        string[len - 1] = '\0';
        len--;
    }
}

static void karadioUpdateName(const char *str)
{
    strncpy(krData.name, str, ST_NAME_SIZE);
    trimEnd(krData.name);
    krData.flags |= KARADIO_FLAG_NAME;
}

static void karadioUpdateMeta(const char *str)
{
    strncpy(krData.meta, str, ST_META_SIZE);
    trimEnd(krData.meta);
    krData.flags |= KARADIO_FLAG_META;
}

static void karadioParseLine(char *line)
{
    if (utilIsPrefix(line, "##CLI.PLAYING#")) {
        krData.flags |= (KARADIO_FLAG_ALL);
    } else if (utilIsPrefix(line, "##CLI.STOPPED#")) {
        karadioUpdateMeta("---");
    } else if (utilIsPrefix(line, "##CLI.META#:")) {
        karadioUpdateMeta(line + sizeof("##CLI.META#:"));
    } else if (utilIsPrefix(line, "##CLI.NAMESET#:")) {
        char *nameset = line + sizeof("##CLI.NAMESET#:");
        char *sp = strstr(nameset, " ");
        if (sp != nameset) {
            karadioUpdateNumber(nameset, (size_t)(sp - nameset));
            karadioUpdateName(sp + 1);
        }
    } else if (utilIsPrefix(line, "Trying ")) {
        char *apName = line + sizeof("Trying ");
        char *cm = strstr(apName, ",");
        if (cm != apName) {
            *cm = '\0';
        }
        karadioUpdateName(line);
        karadioUpdateMeta(cm + 1);
    } else if (utilIsPrefix(line, "ip:")) {
        char *ip = line + sizeof("ip:");
        char *cm = strstr(ip, ",");
        if (cm != ip) {
            *cm = '\0';
        }
        karadioUpdateMeta(line);
    } else if (utilIsPrefix(line, "I2S Speed:")) {
        AudioProc *aProc = audioGet();
        InputType inType = aProc->par.inType[aProc->par.input];
        if ((ampGet()->status == AMP_STATUS_ACTIVE) && (inType == IN_KARADIO)) {
            karadioSendCmdCli("start");
        } else {
            karadioSendCmdCli("stop");
        }
    }
}

void karadioGetData(void)
{
    uint16_t size = ringBufGetSize(&krRingBuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&krRingBuf);
        if (utilReadChar(&krLp, ch)) {
            karadioParseLine(krLp.line);
        }
    }
}

void USART_KARADIO_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_KARADIO) && LL_USART_IsEnabledIT_RXNE(USART_KARADIO)) {
        char data = LL_USART_ReceiveData8(USART_KARADIO);
#ifdef _DEBUG_KARADIO
        usartSendChar(USART_DBG, data);
#endif

        ringBufPushChar(&krRingBuf, data);
    } else {
        // Call Error function
    }
}

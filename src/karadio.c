#include "karadio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amp.h"
#include "audio/audio.h"
#include "debug.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"

#define KARADIO_RB_SIZE 128

static KaRadio kaRadio;

static RingBuf rbuf;
static char rbData[KARADIO_RB_SIZE];
static LineParse lp;

static void karadioUpdateNumber(const char *str, size_t numLen);
static void karadioUpdateName(const char *str);
static void karadioUpdateMeta(const char *str);

static void karadioSendCmdCli(const char *cmd)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "\ncli.%s\n", cmd);
    usartSendString(USART_KARADIO, buf);
}

void karadioInit(void)
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));

    usartInit(USART_KARADIO, 115200);
    usartSetRxIRQ(USART_KARADIO, true);
}

KaRadio *karadioGet(void)
{
    return &kaRadio;
}

void karadioPlayStation(int16_t num)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "\ncli.play(\"%d\")\n", num);
    usartSendString(USART_KARADIO, buf);
}

void kaRadioSendDigit(uint8_t dig)
{
    if (kaRadio.station < 0) {
        kaRadio.station = dig;
    } else {
        kaRadio.station = kaRadio.station * 10 + dig;
    }

    kaRadio.station %= 1000;

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", kaRadio.station);
    karadioUpdateNumber(buf, 4);
}

void kaRadioFinishDigitInput(void)
{
    karadioPlayStation(kaRadio.station);
    kaRadio.station = -1;
}

void karadioSendMediaKey(MediaKey key)
{
    switch (key) {
    case MEDIAKEY_STOP:
        karadioSendCmdCli("stop");
        break;
    case MEDIAKEY_PLAY:
        if (kaRadio.flags & KARADIO_FLAG_PLAYING) {
            karadioSendCmdCli("stop");
        } else {
            karadioSendCmdCli("start");
        }
        break;
    case MEDIAKEY_PREV:
        karadioSendCmdCli("prev");
        break;
    case MEDIAKEY_NEXT:
        karadioSendCmdCli("next");
        break;
    default:
        break;
    }
}

static void karadioUpdateNumber(const char *str, size_t numLen)
{
    if (numLen > ST_NUM_SIZE) {
        numLen = ST_NUM_SIZE;
    }
    memset(kaRadio.num, 0, ST_NUM_SIZE);
    strncpy(kaRadio.num, str, numLen);
    kaRadio.flags |= KARADIO_FLAG_NUMBER;
}

static void karadioUpdateName(const char *str)
{
    strncpy(kaRadio.name, str, ST_NAME_SIZE);
    utilTrimLineEnd(kaRadio.name);
    kaRadio.flags |= KARADIO_FLAG_NAME;
}

static void karadioUpdateMeta(const char *str)
{
    strncpy(kaRadio.meta, str, ST_META_SIZE);
    utilTrimLineEnd(kaRadio.meta);
    kaRadio.flags |= KARADIO_FLAG_META;
}

static void karadioParseCli(char *line)
{
    // Follows "##CLI."
    if (utilIsPrefix(line, "PLAYING#")) {
        kaRadio.flags |= KARADIO_FLAG_ALL;
    } else if (utilIsPrefix(line, "STOPPED#")) {
        kaRadio.flags &= ~KARADIO_FLAG_PLAYING;
        karadioUpdateMeta("---");
    } else if (utilIsPrefix(line, "META#:")) {
        karadioUpdateMeta(line + sizeof("META#:"));
    } else if (utilIsPrefix(line, "NAMESET#:")) {
        char *nameset = line + sizeof("NAMESET#:");
        char *sp = strstr(nameset, " ");
        if (sp != nameset) {
            karadioUpdateNumber(nameset, (size_t)(sp - nameset));
            karadioUpdateName(sp + 1);
        }
    }
}

static void karadioParseApTrying(char *line)
{
    // Follows "Trying "
    char *apName = line;
    char *cm = strstr(apName, ",");
    if (cm != apName) {
        *cm = '\0';
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "AP: %s", line);
    karadioUpdateName(buf);
    karadioUpdateMeta(cm + 1);
}

static void karadioParseIP(char *line)
{
    // Follows "ip:"
    char *ip = line;
    char *cm = strstr(ip, ",");
    if (cm != ip) {
        *cm = '\0';
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "IP: %s", line);
    karadioUpdateMeta(buf);
}

static void karadinOnBootComplete()
{
    // Follows "I2S Speed:"
    Amp *amp = ampGet();
    AudioProc *aProc = audioGet();
    InputType inType = amp->inType[aProc->par.input];

    if ((amp->status == AMP_STATUS_ACTIVE) && (inType == IN_KARADIO)) {
        karadioSendCmdCli("start");
    } else {
        karadioSendCmdCli("stop");
    }
}

static void karadioParseLine(char *line)
{
    if (utilIsPrefix(line, "##CLI.")) {
        karadioParseCli(line + strlen("##CLI."));
    } else if (utilIsPrefix(line, "Trying ")) {
        karadioParseApTrying(line + strlen("Trying "));
    } else if (utilIsPrefix(line, "ip:")) {
        karadioParseIP(line + strlen("ip:"));
    } else if (utilIsPrefix(line, "I2S Speed:")) {
        karadinOnBootComplete();
    }
}

void karadioGetData(void)
{
    uint16_t size = ringBufGetSize(&rbuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&rbuf);
        if (utilReadChar(&lp, ch)) {
            karadioParseLine(lp.line);
        }
    }
}

void USART_KARADIO_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_KARADIO) && LL_USART_IsEnabledIT_RXNE(USART_KARADIO)) {
        char data = LL_USART_ReceiveData8(USART_KARADIO);
        ringBufPushChar(&rbuf, data);
    } else {
        // Call Error function
    }
}

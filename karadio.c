#include "karadio.h"

#include <string.h>

#include "actions.h"
#include "amp.h"
#include "audio/audio.h"
#include "debug.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "usart.h"
#include "usb/hidkeys.h"
#include "utils.h"

#define CMD_CLI         "cli"
#define CMD_SYS         "sys"
#define CMD_WIFI        "wifi"

#define CLI_PLAY        "start"
#define CLI_STOP        "stop"
#define CLI_PREV        "prev"
#define CLI_NEXT        "next"
#define CLI_INFO        "info"

#define CLI_META        "##CLI.META#:"
#define CLI_NAMESET     "##CLI.NAMESET#:"
#define CLI_STOPPED     "##CLI.STOPPED#"
#define CLI_PLAYING     "##CLI.PLAYING#"

#define TRYING          "Trying "
#define IP              "ip:"
#define AUTOSTART       "autostart:"

#define SYS_BOOT        "boot"
#define WIFI_DISCON     "discon"

#define KARADIO_RB_SIZE 128

static KaRadioData krData;
static bool kEnabled = false;

static RingBuf krRingBuf;
static char krRbData[KARADIO_RB_SIZE];
static LineParse krLp;

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
        if (!kEnabled) {
            karadioSendCmd(CMD_SYS, SYS_BOOT);
        }
    } else {
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

static void karadioUpdateNumber(const char *str, size_t numLen)
{
    if (numLen > ST_NUM_SIZE)
        numLen = ST_NUM_SIZE;
    strncpy(krData.num, str, numLen);
    krData.num[numLen] = '\0';
}

static void karadioUpdateName(const char *str)
{
    strncpy(krData.name, str, ST_NAME_SIZE);
    krData.flags |= KARADIO_FLAG_NAME;
}

static void karadioUpdateMeta(const char *str)
{
    strncpy(krData.meta, str, ST_META_SIZE);
    krData.flags |= KARADIO_FLAG_META;
}

static void karadioParseLine(char *line)
{
    if (strstr(line, CLI_PLAYING) == line) {
        krData.flags |= (KARADIO_FLAG_ALL);
    } else if (strstr(line, CLI_STOPPED) == line) {
        karadioUpdateMeta("---");
    } else if (strstr(line, CLI_META) == line) {
        karadioUpdateMeta(line + sizeof(CLI_META));
    } else if (strstr(line, CLI_NAMESET) == line) {
        char *nameset = line + sizeof(CLI_NAMESET);
        char *sp = strstr(nameset, " ");
        if (sp != nameset) {
            karadioUpdateNumber(nameset, (size_t)(sp - nameset));
            karadioUpdateName(sp + 1);
        }
    } else if (strstr(line, TRYING) == line) {
        char *apName = line + sizeof(TRYING);
        char *cm = strstr(apName, ",");
        if (cm != apName) {
            *cm = '\0';
        }
        karadioUpdateName(line);
        karadioUpdateMeta(cm + 1);
    } else if (strstr(line, IP) == line) {
        char *ip = line + sizeof(IP);
        char *cm = strstr(ip, ",");
        if (cm != ip) {
            *cm = '\0';
        }
        karadioUpdateMeta(line);
    } else if (strstr(line, AUTOSTART) == line) {
        AudioProc *aProc = audioGet();
        InputType inType = aProc->par.inType[aProc->par.input];
        if ((ampGetStatus() == AMP_STATUS_ACTIVE) && (inType == IN_KARADIO)) {
            karadioSendCmd(CMD_CLI, CLI_PLAY);
        } else {
            karadioSendCmd(CMD_CLI, CLI_STOP);
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

void karadioIRQ(char data)
{
#ifdef _DEBUG_KARADIO
    usartSendChar(USART_DBG, data);
#endif

    ringBufPushChar(&krRingBuf, data);
}

#include "mpc.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amp.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"

#define RING_BUF_SIZE 256

static Mpc mpc;

static RingBuf rbuf;
static char rbData[RING_BUF_SIZE];
static LineParse lp;

static void mpcSendCmd(const char *cmd)
{
    const char *prefix = "\ncli.";

    char buf[32];
    snprintf(buf, sizeof(buf), "%s%s\r\n", prefix, cmd);
    usartSendString(USART_MPC, buf);
}

static void updateMeta(const char *str)
{
    strncpy(mpc.meta, str, MPC_META_SIZE);
    utilTrimLineEnd(mpc.meta);
    mpc.flags |= MPC_FLAG_UPDATE_META;
}

static void updateName(const char *str) // KaRadio only
{
    strncpy(mpc.name, str, MPC_NAME_SIZE);
    utilTrimLineEnd(mpc.name);
    mpc.flags |= MPC_FLAG_UPDATE_NAME;
}

static void parseCli(char *line)
{
    if (utilIsPrefix(line, "ELAPSED#: ")) {
        int elapsed;
        sscanf(line, "ELAPSED#: %d", &elapsed);
        mpc.elapsed = elapsed;
        mpc.flags |= MPC_FLAG_UPDATE_ELAPSED;
    } else if (utilIsPrefix(line, "DURATION#: ")) {
        int duration;
        sscanf(line, "DURATION#: %d", &duration);
        mpc.duration = duration;
        mpc.flags |= MPC_FLAG_UPDATE_DURATION;
        mpc.flags |= MPC_FLAG_UPDATE_META;
    } else if (utilIsPrefix(line, "META#: ")) {
        updateMeta(line + strlen("META#: "));
    } else if (utilIsPrefix(line, "PLAYING#")) {
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
        mpc.flags |= MPC_FLAG_UPDATE_NAME | MPC_FLAG_UPDATE_META | MPC_FLAG_UPDATE_TRACKNUM;
        mpc.status |= MPC_PLAYING;
        mpc.status &= ~MPC_PAUSED;
    } else if (utilIsPrefix(line, "PAUSED#")) {
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
        mpc.status |= MPC_PAUSED;
    } else if (utilIsPrefix(line, "STOPPED#")) {
        mpc.flags |= (MPC_FLAG_UPDATE_STATUS | MPC_FLAG_UPDATE_ELAPSED);
        mpc.status &= ~(MPC_PLAYING | MPC_PAUSED);
    } else if (utilIsPrefix(line, "REPEAT#: ")) {
        int repeat;
        sscanf(line, "REPEAT#: %d", &repeat);
        repeat ? (mpc.status |= MPC_REPEAT) : (mpc.status &= ~MPC_REPEAT);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "RANDOM#: ")) {
        int random;
        sscanf(line, "RANDOM#: %d", &random);
        random ? (mpc.status |= MPC_RANDOM) : (mpc.status &= ~MPC_RANDOM);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "SINGLE#: ")) {
        int single;
        sscanf(line, "SINGLE#: %d", &single);
        single ? (mpc.status |= MPC_SINGLE) : (mpc.status &= ~MPC_SINGLE);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "CONSUME#: ")) {
        int consume;
        sscanf(line, "CONSUME#: %d", &consume);
        consume ? (mpc.status |= MPC_CONSUME) : (mpc.status &= ~MPC_CONSUME);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "NAMESET#:")) {
        char *name = line + sizeof("NAMESET#:");
        updateName(name);
    }
}

static void parseApTrying(char *line) // KaRadio only
{
    // Follows "Trying "
    char *apName = line;
    char *cm = strstr(apName, ",");
    if (cm != apName) {
        *cm = '\0';
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "AP: %s", line);
    updateName(buf);
    updateMeta(cm + 1);
}

static void parseIP(char *line)
{
    // Follows "ip:"
    char *ip = line;
    char *cm = strstr(ip, ",");
    if (!cm) {
        cm = line;
    }
    if (cm != ip) {
        *cm = '\0';
    }
    char buf[40];
    snprintf(buf, sizeof(buf), "IP: %s", line);
    updateMeta(buf);
}

static void onBootComplete() // KaRadio only
{
    // Follows "I2S Speed:"
    Amp *amp = ampGet();

    if (amp->status == AMP_STATUS_ACTIVE) {
        mpcSendCmd("start");
    } else {
        mpcSendCmd("stop");
    }
}

static void parseLine(char *line)
{
    if (utilIsPrefix(line, "##CLI.")) {
        parseCli(line + strlen("##CLI."));
    } else if (utilIsPrefix(line, "Trying ")) {
        parseApTrying(line + strlen("Trying "));
    } else if (utilIsPrefix(line, "ip:")) {
        parseIP(line + strlen("ip:"));
    } else if (utilIsPrefix(line, "I2S Speed:")) {
        onBootComplete();
    }
}

void mpcInit(void)
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));
    mpc.trackNum = -1;

    usartInit(USART_MPC, 115200);
    usartSetRxIRQ(USART_MPC, true);
}

Mpc *mpcGet(void)
{
    return &mpc;
}

void mpcSyncRequest(void)
{
    mpcSendCmd("info");
}

void mpcSendDigit(uint8_t dig)
{
    if (mpc.trackNum < 0) {
        mpc.trackNum = dig;
    } else {
        mpc.trackNum = mpc.trackNum * 10 + dig;
    }

    mpc.trackNum %= 1000;
    mpc.trackNum = mpc.trackNum;

    char buf[32];
    snprintf(buf, sizeof(buf), "%" PRId32, mpc.trackNum);
    mpc.flags |= MPC_FLAG_UPDATE_TRACKNUM;
}

void mpcFinishDigitInput(void)
{
    mpcPlayTrack(mpc.trackNum);
    mpc.trackNum = -1;
    mpc.flags |= MPC_FLAG_UPDATE_TRACKNUM;
}

void mpcLoadPlaylist(int16_t num)
{
    mpc.trackNum = -1;
    mpc.flags |= MPC_FLAG_UPDATE_TRACKNUM;

    if (num < 0 || num > 999) {
        return;
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "load(\"Playlist_%03d\")", num);
    mpcSendCmd(buf);
}

void mpcPlayTrack(int16_t num)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "\ncli.play(\"%d\")\n", num);
    usartSendString(USART_MPC, buf);
}

void mpcSendMediaKey(MediaKey key)
{
    switch (key) {
    case MEDIAKEY_PLAY:
        mpcSendCmd("start");
        break;
    case MEDIAKEY_PAUSE:
        mpcSendCmd("pause");
        break;
    case MEDIAKEY_STOP:
        mpcSendCmd("stop");
        break;
    case MEDIAKEY_PREV:
        mpcSendCmd("previous");
        break;
    case MEDIAKEY_NEXT:
        mpcSendCmd("next");
        break;
    case MEDIAKEY_REWIND:
        mpcSendCmd("rewind");
        break;
    case MEDIAKEY_FFWD:
        mpcSendCmd("ffwd");
        break;
    case MEDIAKEY_REPEAT:
        mpcSendCmd("repeat");
        break;
    case MEDIAKEY_RANDOM:
        mpcSendCmd("random");
        break;
    case MEDIAKEY_SINGLE:
        mpcSendCmd("single");
        break;
    case MEDIAKEY_CONSUME:
        mpcSendCmd("consume");
        break;
    case MEDIAKEY_EJECT:
        mpcSendCmd("load(\"Music\")");
        break;
    case MEDIAKEY_INJECT:
        mpcSendCmd("load(\"Radio\")");
        break;
    default:
        break;
    }
}

void mpcGetData(void)
{
    uint16_t size = ringBufGetSize(&rbuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&rbuf);
        if (utilReadChar(&lp, ch)) {
            parseLine(lp.line);
        }
    }
}

void mpcSchedPower(bool value)
{
    if (!value) {
        mpcSendCmd("poweroff");
    }
}

void USART_MPC_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_MPC) && LL_USART_IsEnabledIT_RXNE(USART_MPC)) {
        char data = LL_USART_ReceiveData8(USART_MPC);
        ringBufPushChar(&rbuf, data);
    } else {
        // Call Error function
    }
}

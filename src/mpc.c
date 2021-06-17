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

static void updateIp(const char *str)
{
    strncpy(mpc.ip, str, IP_STR_SIZE);
    utilTrimLineEnd(mpc.ip);
    mpc.flags |= MPC_FLAG_UPDATE_META;
    mpc.status |= MPC_ONLINE;
}

static void updateName(const char *str) // KaRadio only
{
    strncpy(mpc.name, str, MPC_NAME_SIZE);
    utilTrimLineEnd(mpc.name);
    mpc.flags |= MPC_FLAG_UPDATE_NAME;
}

static void mpcReset(void)
{
    mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    mpc.status = MPC_IDLE;
}

static void mpcSetMode(const char *str)
{
    char buf[8];
    strncpy(buf, str, sizeof(buf));
    utilTrimLineEnd(buf);

    if (!strcmp(buf, "BLUEZ")) {
        mpc.status |= MPC_BT_ON;
    } else if (!strcmp(buf, "MPD")) {
        mpc.status &= ~MPC_BT_ON;
    }
    updateMeta("");
    mpc.status &= ~(MPC_PLAYING | MPC_PAUSED);
    mpc.elapsed = -1;
    mpc.flags |= (MPC_FLAG_UPDATE_STATUS | MPC_FLAG_UPDATE_ELAPSED);
}

static void parseSys(char *line)
{
    if (utilIsPrefix(line, "DATE#:")) {
        char *date = line + sizeof("DATE#:");
        ampUpdateDate(date);
    } else if (utilIsPrefix(line, "MODE#:")) {
        char *mode = line + sizeof("MODE#:");
        mpcSetMode(mode);
    } else if (utilIsPrefix(line, "RESET")) {
        mpcReset();
    }
}

static void parseCli(char *line)
{
    int ret;

    if (utilIsPrefixInt(line, "ELAPSED#: ", &ret)) {
        mpc.elapsed = ret;
        mpc.flags |= MPC_FLAG_UPDATE_ELAPSED;
    } else if (utilIsPrefixInt(line, "DURATION#: ", &ret)) {
        mpc.duration = ret;
        mpc.flags |= MPC_FLAG_UPDATE_DURATION;
        mpc.flags |= MPC_FLAG_UPDATE_META;
    } else if (utilIsPrefix(line, "META#: ")) {
        updateMeta(line + strlen("META#: "));
    } else if (utilIsPrefix(line, "NAMESET#:")) {
        char *name = line + sizeof("NAMESET#:");
        updateName(name);
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
    } else if (utilIsPrefixInt(line, "REPEAT#: ", &ret)) {
        ret ? (mpc.status |= MPC_REPEAT) : (mpc.status &= ~MPC_REPEAT);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefixInt(line, "RANDOM#: ", &ret)) {
        ret ? (mpc.status |= MPC_RANDOM) : (mpc.status &= ~MPC_RANDOM);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefixInt(line, "SINGLE#: ", &ret)) {
        ret ? (mpc.status |= MPC_SINGLE) : (mpc.status &= ~MPC_SINGLE);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefixInt(line, "CONSUME#: ", &ret)) {
        ret ? (mpc.status |= MPC_CONSUME) : (mpc.status &= ~MPC_CONSUME);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    }
}

static void parseApTrying(char *line) // KaRadio only
{
    // Follows "Trying "
    char *comma = strstr(line, ",");

    size_t apLen = 32;
    if (comma != line) {
        apLen = comma - line;
    }

    char buf[32] = "AP: ";
    strncat(buf, line, apLen);
    updateName(buf);
    updateMeta(comma + 1);
}

static void parseWiFi(char *line) // KaRadio32 only
{
    updateName(line);
}

static void parseDNS(char *line) // KaRadio32 only
{
    updateName(line);
}

static void parseIP(char *line)
{
    char *pos = line;

    strtol(pos, &pos, 10);
    if (*pos != '.') {
        return;
    }
    strtol(pos+1, &pos, 10);
    if (*pos != '.') {
        return;
    }
    strtol(pos+1, &pos, 10);
    if (*pos != '.') {
        return;
    }
    strtol(pos+1, &pos, 10);

    char buf[24] = "IP: ";
    strncat(buf, line, pos-line);
    updateIp(buf);
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
    } else if (utilIsPrefix(line, "##SYS.")) {
        parseSys(line + strlen("##SYS."));
    } else if (utilIsPrefix(line, "Trying ")) {
        parseApTrying(line + strlen("Trying "));
    } else if (utilIsPrefix(line, "ip:")) {
        parseIP(line + strlen("ip:"));
    } else if (utilIsPrefix(line, "IP: ")) {
        parseIP(line + strlen("IP: "));
    } else if (utilIsPrefix(line, "I2S Speed:")) {
        onBootComplete();
    } else if (utilIsPrefix(line, "rst:")) {
        mpcReset();
    } else if (utilIsPrefix(line, "WIFI ")) {
        parseWiFi(line + strlen("WIFI "));
    } else if (utilIsPrefix(line, "DNS: ")) {
        parseDNS(line);
    }
}

void mpcInit(void)
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));
    mpc.trackNum = -1;

    usartInit(USART_MPC, 115200);
    usartSetRxIRQ(USART_MPC, true);

    mpcReset();
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
        mpcReset();
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

void mpcSetBluetooth(bool value)
{
    if (value) {
        mpcSendCmd("mode(\"bluez\")");
    } else {
        mpcSendCmd("mode(\"mpd\")");
    }
    mpc.flags |= MPC_FLAG_UPDATE_STATUS;
}

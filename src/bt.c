#include "bt.h"

#include "amp.h"
#include "i2cexp.h"
#include "hwlibs.h"
#include "menu.h"
#include "ringbuf.h"
#include <usart.h>
#include "utils.h"

#include <string.h>

#define CMDBUF_SIZE     256

static RingBuf rbuf;
static char rbData[CMDBUF_SIZE];
static LineParse lp;

static BTCtx btCtx;

static void btPlay()
{
    usartSendString(USART_BT, "AT+CB\r\n");
    i2cExpGpioKeyPress(BT_PLAY_PAUSE);
}

static void btPrevTrack(void)
{
    usartSendString(USART_BT, "AT+CD\r\n");
    i2cExpGpioKeyPress(BT_PREV_TRACK);
}

static void btNextTrack(void)
{
    usartSendString(USART_BT, "AT+CC\r\n");
    i2cExpGpioKeyPress(BT_NEXT_TRACK);
}

static void utf16To8(char *ustr, char *str, int16_t size)
{
    for (int16_t i = 0; i < size; i++) {
        uint16_t ucs = (uint16_t)((ustr[2 * i]) | (ustr[2 * i + 1] << 8));

        if (ucs <= 0x007F) {
            *str++ = 0x00 | (char)((ucs >> 0)  & 0x7F);
        } else if (ucs <= 0x7FF) {
            *str++ = 0xC0 | (char)((ucs >> 6)  & 0x1F);
            *str++ = 0x80 | (char)((ucs >> 0)  & 0x3F);
        } else {
            *str++ = 0xE0 | (char)((ucs >> 12) & 0x0F);
            *str++ = 0x80 | (char)((ucs >> 6)  & 0x3F);
            *str++ = 0x80 | (char)((ucs >> 0)  & 0x3F);
        }
    }
    *str = 0;
}

BTCtx *btCtxGet(void)
{
    return &btCtx;
}

void btSendMediaKey(MediaKey key)
{
    switch (key) {
    case MEDIAKEY_PLAY:
        btPlay();
        break;
    case MEDIAKEY_PREV:
        btPrevTrack();
        break;
    case MEDIAKEY_NEXT:
        btNextTrack();
        break;
    default:
        break;
    }
}

BtInput btGetInput(void)
{
    return btCtx.input;
}

void btAddInput(BtInput value)
{
    btCtx.inMask |= value;
}

void btDelInput(BtInput value)
{
    btCtx.inMask &= ~value;
    if (btCtx.input & value) {
        if (btCtx.inMask & BT_IN_USB) {
            btSetInput(BT_IN_USB);
        } else if (btCtx.inMask & BT_IN_SDCARD) {
            btSetInput(BT_IN_SDCARD);
        } else {
            btSetInput(BT_IN_BLUETOOTH);
        }
    }
}

void btSetInput(BtInput value)
{
    btCtx.inMask |= value;
    btCtx.input = value;
    btCtx.flags |= BT_FLAG_NAME_CHANGED;
}

void btNextInput()
{
    usartSendString(USART_BT, "AT+CM00\r\n");
    i2cExpGpioKeyPress(BT_NEXT_INPUT);
}

char *btGetSongName(void)
{
    if (btCtx.input == BT_IN_BLUETOOTH) {
        return "";
    } else {
        return btCtx.songName;
    }
}

void bt201ParseMount(char *line)
{
    // Follows "MU+"
    if (utilIsPrefix(line, "01")) {
        btAddInput(BT_IN_USB);
    } else if (utilIsPrefix(line, "02")) {
        btDelInput(BT_IN_USB);
    } else if (utilIsPrefix(line, "03")) {
        btAddInput(BT_IN_SDCARD);
    } else if (utilIsPrefix(line, "04")) {
        btDelInput(BT_IN_SDCARD);
    }
}

void bt201ParseInput(char *line)
{
    // Follows "QM+"
    if (utilIsPrefix(line, "01")) {
        btSetInput(BT_IN_BLUETOOTH);
    } else if (utilIsPrefix(line, "02")) {
        btSetInput(BT_IN_USB);
    } else if (utilIsPrefix(line, "03")) {
        btSetInput(BT_IN_SDCARD);
    } else if (utilIsPrefix(line, "00")) {
        btDelInput(BT_IN_USB | BT_IN_SDCARD);
    }
}

void bt201ParseSongName(char *line, int16_t size)
{
    // Follows "MF+"
    char *buffer = btCtx.songName;

    utf16To8(line, buffer, size / 2);

    // Remove extension
    size_t dotPos = strlen(buffer) - 4;
    // 3 chars (mp3, wav, wma, aac, ape)
    buffer[dotPos] = '\0';
    if (buffer[dotPos - 1] == '.') {
        // 4 chars (flac)
        buffer[dotPos - 1] = '\0';
    }

    btCtx.flags |= BT_FLAG_NAME_CHANGED;

    // Query current mode
    usartSendString(USART_BT, "AT+QM\r\n");

    return;
}

static void controlParseLine(LineParse *lp)
{
    char *line = lp->line;

    // BT201 control
    if (utilIsPrefix(line, "QM+")) {
        bt201ParseInput(line + strlen("QM+"));
        if (btGetInput() & (BT_IN_USB | BT_IN_SDCARD)) {
            ampActionQueue(ACTION_AUDIO_INPUT_SET_TYPE, IN_BLUETOOTH);
        }
    } else if (utilIsPrefix(line, "MU+")) {
        bt201ParseMount(line + strlen("MU+"));
    } else if (utilIsPrefix(line, "MF+")) {
        bt201ParseSongName(line + strlen("MF+"), lp->size - (int16_t)strlen("MF+"));
    }
}

void btInit(void)
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));

    usartInit(USART_BT, 115200);
    usartSetRxIRQ(USART_BT, true);
    usartSendChar(USART_BT, '\r');
}

void USART_BT_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_BT) && LL_USART_IsEnabledIT_RXNE(USART_BT)) {
        char ch = LL_USART_ReceiveData8(USART_BT);
        ringBufPushChar(&rbuf, ch);
    } else {
        // Call Error function
    }
}

void btGetData(void)
{
    while (ringBufGetSize(&rbuf) > 0) {
        char ch = ringBufPopChar(&rbuf);
        if (utilReadChar(&lp, ch)) {
            if (lp.line[0] != '\0') {
                controlParseLine(&lp);
            }
        }
    }
}


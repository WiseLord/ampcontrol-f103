#include "bt.h"

#include "debug.h"
#include "i2cexp.h"
#include "menu.h"
#include "string.h"
#include "utils.h"

static BTCtx btCtx;

static void btPlay()
{
    dbg("AT+CB");
    i2cExpGpioKeyPress(BT_PLAY_PAUSE);
}

static void btPrevTrack(void)
{
    dbg("AT+CD");

    i2cExpGpioKeyPress(BT_PREV_TRACK);
}

static void btNextTrack(void)
{
    dbg("AT+CC");

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
    dbg("AT+CM00");
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
    dbg("AT+QM");

    return;
}

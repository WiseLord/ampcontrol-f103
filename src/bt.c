#include "bt.h"

#include "debug.h"
#include "i2c.h"
#include "i2cexp.h"
#include "settings.h"
#include "string.h"
#include "swtimers.h"
#include "utils.h"

#define PCF8574_RELEASED    0x00

//#define PCF8574_VOL_MINUS   (1 << 7)
//#define PCF8574_VOL_PLUS    (1 << 6)
#define PCF8574_NEXT_TRACK  (1 << 5)
#define PCF8574_PREV_TRACK  (1 << 4)
#define PCF8574_NEXT_INPUT  (1 << 3)
#define PCF8574_PLAY_PAUSE  (1 << 2)
//#define PCF8574_LED1        (1 << 1)
//#define PCF8574_LED0        (1 << 0)

static I2cAddrIdx i2cAddrIdx = I2C_ADDR_DISABLED;

static BtInput btInputs = BT_IN_BLUETOOTH;
static BtInput btInput = BT_IN_BLUETOOTH;

static BTCtx btCtx;

static void btStartKeyTimer(void)
{
    swTimSet(SW_TIM_BT_KEY, 200);
}

static void btPlay()
{
    dbg("AT+CB");
    i2cexpSend(i2cAddrIdx, PCF8574_PLAY_PAUSE);
    btStartKeyTimer();
}

static void btPrevTrack(void)
{
    dbg("AT+CD");

    i2cexpSend(i2cAddrIdx, PCF8574_PREV_TRACK);
    btStartKeyTimer();
}

static void btNextTrack(void)
{
    dbg("AT+CC");

    i2cexpSend(i2cAddrIdx, PCF8574_NEXT_TRACK);
    btStartKeyTimer();
}


static void utf16To8(char *ustr, char *str, int16_t size)
{
    for (int16_t i = 0; i < size; i++) {
        uint16_t ucs = (uint16_t)((ustr[2 * i]) | (ustr[2 * i + 1] << 8));

        if (ucs <= 0x007F) {
            *str++ = (char)(((ucs >> 0) & 0x7F) | 0x00);
        } else if (ucs <= 0x7FF) {
            *str++ = (char)(((ucs >> 6) & 0x1F) | 0xC0);
            *str++ = (char)(((ucs >> 0) & 0x3F) | 0x80);
        } else {
            *str++ = (char)(((ucs >> 12) & 0x0F) | 0xE0);
            *str++ = (char)(((ucs >> 6) & 0x1F) | 0xC0);
            *str++ = (char)(((ucs >> 0) & 0x3F) | 0x80);
        }
    }
    *str = 0;
}

void btInit(void)
{
    i2cAddrIdx = (I2cAddrIdx)settingsGet(PARAM_I2C_EXT_BT);

    if (i2cAddrIdx != I2C_ADDR_DISABLED) {
        i2cexpSend(i2cAddrIdx, PCF8574_RELEASED);
    }
}

BTCtx *btCtxGet(void)
{
    return &btCtx;
}

void btSendMediaKey(HidMediaKey cmd)
{
    switch (cmd) {
    case HIDMEDIAKEY_PLAY:
        btPlay();
        break;
    case HIDMEDIAKEY_PREV_TRACK:
        btPrevTrack();
        break;
    case HIDMEDIAKEY_NEXT_TRACK:
        btNextTrack();
        break;
    default:
        break;
    }
}

void btReleaseKey(void)
{
    if (swTimGet(SW_TIM_BT_KEY) == 0) {
        i2cexpSend(i2cAddrIdx, PCF8574_RELEASED);
        swTimSet(SW_TIM_BT_KEY, SW_TIM_OFF);
    }
}

BtInput btGetInput(void)
{
    return btInput;
}

void btAddInput(BtInput value)
{
    btInputs |= value;
}

void btDelInput(BtInput value)
{
    btInputs &= ~value;
    if (btInput & value) {
        if (btInputs & BT_IN_USB) {
            btSetInput(BT_IN_USB);
        } else if (btInputs & BT_IN_SDCARD) {
            btSetInput(BT_IN_SDCARD);
        } else {
            btSetInput(BT_IN_BLUETOOTH);
        }
    }
}

void btSetInput(BtInput value)
{
    btInputs |= value;
    btInput = value;
    btCtx.flags |= BT_FLAG_NAME_CHANGED;
}

void btNextInput()
{
    dbg("AT+CM00");
    i2cexpSend(i2cAddrIdx, PCF8574_NEXT_INPUT);
    btStartKeyTimer();
}

char *btGetSongName(void)
{
    if (btInput == BT_IN_BLUETOOTH) {
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

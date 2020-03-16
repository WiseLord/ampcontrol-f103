#include "bt.h"

#include "hwlibs.h"

#include "debug.h"
#include "i2c.h"
#include "i2cexp.h"
#include "settings.h"
#include "swtimers.h"

#define PCF8574_RELEASED    0x00

//#define PCF8574_VOL_MINUS   (1 << 7)
//#define PCF8574_VOL_PLUS    (1 << 6)
#define PCF8574_NEXT_TRACK  (1 << 5)
#define PCF8574_PREV_TRACK  (1 << 4)
//#define PCF8574_POWER       (1 << 3)
#define PCF8574_PLAY_PAUSE  (1 << 2)
//#define PCF8574_LED1        (1 << 1)
//#define PCF8574_LED0        (1 << 0)

static I2cAddrIdx i2cAddrIdx = I2C_ADDR_DISABLED;

static BtInput btInputs = BT_IN_BLUETOOTH;
static BtInput btInput = BT_IN_BLUETOOTH;

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

void btInit(void)
{
    i2cAddrIdx = (I2cAddrIdx)settingsGet(PARAM_I2C_EXT_BT);

    if (i2cAddrIdx != I2C_ADDR_DISABLED) {
        i2cexpSend(i2cAddrIdx, PCF8574_RELEASED);
    }
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
            btInput = BT_IN_USB;
        } else if (btInputs & BT_IN_SDCARD) {
            btInput = BT_IN_SDCARD;
        } else {
            btInput = BT_IN_BLUETOOTH;
        }
    }
}

void btSetInput(BtInput value)
{
    btInputs |= value;
    btInput = value;
}

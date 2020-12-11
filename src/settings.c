#include "settings.h"

#include "audio/audiodefs.h"
#include "display/glcd.h"
#include "eemul.h"
#include "i2cexp.h"
#include "input.h"
#include "rc.h"
#include "spectrum.h"
#include "tr/labels.h"
#include "tuner/tunerdefs.h"

#define GENERATE_EE_RC_MAP(CMD)  [PARAM_RC_ ## CMD] = {0x80 + RC_CMD_ ## CMD, (int16_t)EE_NOT_FOUND},

static const EE_Cell eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_AUDIO_IC]        =   {0x01,  AUDIO_IC_TDA7439},
    [PARAM_AUDIO_INPUT]     =   {0x02,  0},
    [PARAM_AUDIO_LOUDNESS]  =   {0x03,  false},
    [PARAM_AUDIO_SURROUND]  =   {0x04,  false},
    [PARAM_AUDIO_EFFECT3D]  =   {0x05,  false},
    [PARAM_AUDIO_BYPASS]    =   {0x06,  false},
    [PARAM_AUDIO_SHOWDB]    =   {0x07,  true},
    [PARAM_AUDIO_MODE]      =   {0x08,  AUDIO_MODE_2_0},

    [PARAM_AUDIO_IN0]       =   {0x10,  IN_TUNER},
    [PARAM_AUDIO_IN1]       =   {0x11,  IN_KARADIO},
    [PARAM_AUDIO_IN2]       =   {0x12,  IN_PC},
    [PARAM_AUDIO_IN3]       =   {0x13,  IN_BLUETOOTH},
    [PARAM_AUDIO_IN4]       =   {0x14,  IN_TV},
    [PARAM_AUDIO_IN5]       =   {0x15,  IN_USB},
    [PARAM_AUDIO_IN6]       =   {0x16,  IN_DVD},
    [PARAM_AUDIO_IN7]       =   {0x17,  IN_MICROPHONE},

    [PARAM_AUDIO_GAIN0]     =   {0x18,  0},
    [PARAM_AUDIO_GAIN1]     =   {0x19,  0},
    [PARAM_AUDIO_GAIN2]     =   {0x1A,  0},
    [PARAM_AUDIO_GAIN3]     =   {0x1B,  0},
    [PARAM_AUDIO_GAIN4]     =   {0x1C,  0},
    [PARAM_AUDIO_GAIN5]     =   {0x1D,  0},
    [PARAM_AUDIO_GAIN6]     =   {0x1E,  0},
    [PARAM_AUDIO_GAIN7]     =   {0x1F,  0},

    [PARAM_AUDIO_VOLUME]    =   {0x20,  -30},
    [PARAM_AUDIO_BASS]      =   {0x21,  2},
    [PARAM_AUDIO_MIDDLE]    =   {0x22,  -1},
    [PARAM_AUDIO_TREBLE]    =   {0x23,  3},
    [PARAM_AUDIO_FRONTREAR] =   {0x24,  0},
    [PARAM_AUDIO_BALANCE]   =   {0x25,  0},
    [PARAM_AUDIO_CENTER]    =   {0x26,  0},
    [PARAM_AUDIO_SUBWOOFER] =   {0x27,  0},
    [PARAM_AUDIO_PREAMP]    =   {0x28,  0},

    [PARAM_TUNER_IC]        =   {0x30,  TUNER_IC_RDA5807},
    [PARAM_TUNER_BAND]      =   {0x31,  TUNER_BAND_FM_US_EUROPE},
    [PARAM_TUNER_STEP]      =   {0x32,  TUNER_STEP_100K},
    [PARAM_TUNER_DEEMPH]    =   {0x33,  TUNER_DEEMPH_50u},
    [PARAM_TUNER_STA_MODE]  =   {0x34,  false},
    [PARAM_TUNER_FMONO]     =   {0x35,  false},
    [PARAM_TUNER_RDS]       =   {0x36,  true},
    [PARAM_TUNER_BASS]      =   {0x37,  false},
    [PARAM_TUNER_VOLUME]    =   {0x38,  TUNER_VOLUME_MAX},
    [PARAM_TUNER_FREQ]      =   {0x39,  9950},

    [PARAM_SPECTRUM_MODE]   =   {0x50,  SP_MODE_STEREO},
    [PARAM_SPECTRUM_PEAKS]  =   {0x51,  true},
    [PARAM_SPECTRUM_GRAD]   =   {0x52,  true},

    [PARAM_DISPLAY_BR_STBY] =   {0x58,  3},
    [PARAM_DISPLAY_BR_WORK] =   {0x59,  LCD_BR_MAX},
    [PARAM_DISPLAY_ROTATE]  =   {0x5A,  false},
    [PARAM_DISPLAY_DEF]     =   {0x5B,  SCREEN_AUDIO_INPUT},
    [PARAM_DISPLAY_PALETTE] =   {0x5C,  PAL_DEFAULT},

    [PARAM_ALARM_HOUR]      =   {0x60,  7},
    [PARAM_ALARM_MINUTE]    =   {0x61,  30},
    [PARAM_ALARM_DAYS]      =   {0x62,  0},

    [PARAM_SYSTEM_LANG]     =   {0x70,  LANG_DEFAULT},
    [PARAM_SYSTEM_SIL_TIM]  =   {0x72,  5},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  4},
    [PARAM_SYSTEM_STBY_LOW] =   {0x75,  false},
    [PARAM_SYSTEM_MUTE_LOW] =   {0x76,  false},

    [PARAM_I2C_EXT_IN_STAT] =   {0x78,  I2C_ADDR_DISABLED},
    [PARAM_I2C_EXT_BT]      =   {0x79,  I2C_ADDR_DISABLED},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

void settingsInit(void)
{
    eeInit(eeMap, PARAM_END);

    for (Param par = PARAM_NULL + 1; par < PARAM_END; par++) {
        settingsSet(par, settingsRead(par));
    }
}

int16_t settingsRead(Param param)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return (int16_t)EE_NOT_FOUND;
    }

    uint16_t value = eeReadRaw(eeMap[param].cell);

    if (value != EE_NOT_FOUND) {
        return (int16_t)value;
    }

    // Return default value if not found
    return eeMap[param].value;
}

void settingsStore(Param param, int16_t value)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return;
    }

    eeUpdateRaw(eeMap[param].cell, (uint16_t)value);
}

const EE_Cell *eeMapGet(void)
{
    return eeMap;
}

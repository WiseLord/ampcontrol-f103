#include "settings.h"

#include "eemul.h"
#include "rc.h"

#define GENERATE_EE_RC_MAP(CMD)  [PARAM_RC_ ## CMD] = 0x80 + RC_CMD_ ## CMD,

static const uint8_t eeMap[] = {
    // Parameter                   EE cell index
    [PARAM_NULL]                 = 0x00,

    [PARAM_AUDIO_IC]             = 0x01,
    [PARAM_AUDIO_INPUT]          = 0x02,
    [PARAM_AUDIO_FLAGS]          = 0x03,
    [PARAM_AUDIO_MODE]           = 0x04,
    [PARAM_AUDIO_SHOWDB]         = 0x05,
    [PARAM_AUDIO_IN_CFG]         = 0x06,

    [PARAM_AUDIO_BASS_FREQ]      = 0x08,
    [PARAM_AUDIO_BASS_QUAL]      = 0x09,
    [PARAM_AUDIO_MIDDLE_FREQ]    = 0x0A,
    [PARAM_AUDIO_MIDDLE_QUAL]    = 0x0B,
    [PARAM_AUDIO_TREBLE_FREQ]    = 0x0C,
    [PARAM_AUDIO_SUB_CUT_FREQ]   = 0x0D,
    [PARAM_AUDIO_LOUD_PEAK_FREQ] = 0x0E,

    [PARAM_AUDIO_IN0]            = 0x10,
    [PARAM_AUDIO_IN1]            = 0x11,
    [PARAM_AUDIO_IN2]            = 0x12,
    [PARAM_AUDIO_IN3]            = 0x13,
    [PARAM_AUDIO_IN4]            = 0x14,
    [PARAM_AUDIO_IN5]            = 0x15,
    [PARAM_AUDIO_IN6]            = 0x16,
    [PARAM_AUDIO_IN7]            = 0x17,

    [PARAM_AUDIO_GAIN0]          = 0x18,
    [PARAM_AUDIO_GAIN1]          = 0x19,
    [PARAM_AUDIO_GAIN2]          = 0x1A,
    [PARAM_AUDIO_GAIN3]          = 0x1B,
    [PARAM_AUDIO_GAIN4]          = 0x1C,
    [PARAM_AUDIO_GAIN5]          = 0x1D,
    [PARAM_AUDIO_GAIN6]          = 0x1E,
    [PARAM_AUDIO_GAIN7]          = 0x1F,

    [PARAM_AUDIO_VOLUME]         = 0x20,
    [PARAM_AUDIO_BASS]           = 0x21,
    [PARAM_AUDIO_MIDDLE]         = 0x22,
    [PARAM_AUDIO_TREBLE]         = 0x23,
    [PARAM_AUDIO_BALANCE]        = 0x24,
    [PARAM_AUDIO_FRONTREAR]      = 0x25,
    [PARAM_AUDIO_CENTER]         = 0x26,
    [PARAM_AUDIO_SUBWOOFER]      = 0x27,
    [PARAM_AUDIO_PREAMP]         = 0x28,
    [PARAM_AUDIO_LOUDNESS]       = 0x29,

    [PARAM_TUNER_IC]             = 0x30,
    [PARAM_TUNER_BAND]           = 0x31,
    [PARAM_TUNER_STEP]           = 0x32,
    [PARAM_TUNER_DEEMPH]         = 0x33,

    [PARAM_TUNER_VOLUME]         = 0x38,
    [PARAM_TUNER_FREQ]           = 0x39,
    [PARAM_TUNER_FLAGS]          = 0x3A,

    [PARAM_TUNER_FAV_0]          = 0x40,
    [PARAM_TUNER_FAV_1]          = 0x41,
    [PARAM_TUNER_FAV_2]          = 0x42,
    [PARAM_TUNER_FAV_3]          = 0x43,
    [PARAM_TUNER_FAV_4]          = 0x44,
    [PARAM_TUNER_FAV_5]          = 0x45,
    [PARAM_TUNER_FAV_6]          = 0x46,
    [PARAM_TUNER_FAV_7]          = 0x47,
    [PARAM_TUNER_FAV_8]          = 0x48,
    [PARAM_TUNER_FAV_9]          = 0x49,

    [PARAM_SPECTRUM_MODE]        = 0x50,
    [PARAM_SPECTRUM_PEAKS]       = 0x51,
    [PARAM_SPECTRUM_GRAD]        = 0x52,

    [PARAM_DISPLAY_BR_STBY]      = 0x58,
    [PARAM_DISPLAY_BR_WORK]      = 0x59,
    [PARAM_DISPLAY_ROTATE]       = 0x5A,
    [PARAM_DISPLAY_DEF]          = 0x5B,
    [PARAM_DISPLAY_PALETTE]      = 0x5C,
    [PARAM_DISPLAY_SCRSAVER]     = 0x5D,

    [PARAM_ALARM_HOUR]           = 0x60,
    [PARAM_ALARM_MINUTE]         = 0x61,
    [PARAM_ALARM_DAYS]           = 0x62,

    [PARAM_SYSTEM_LANG]          = 0x70,
    [PARAM_SYSTEM_SIL_TIM]       = 0x72,
    [PARAM_SYSTEM_RTC_CORR]      = 0x73,
    [PARAM_SYSTEM_ENC_RES]       = 0x74,
    [PARAM_SYSTEM_STBY_LOW]      = 0x75,
    [PARAM_SYSTEM_MUTE_LOW]      = 0x76,

    [PARAM_I2C_EXT_IN_STAT]      = 0x78,
    [PARAM_I2C_EXT_GPIO]         = 0x79,

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

void settingsInit(void)
{
    eeInit(eeMap, PARAM_END);
}

int16_t settingsRead(Param param, int16_t defValue)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return (int16_t)EE_NOT_FOUND;
    }

    uint16_t value = eeReadRaw(eeMap[param]);

    if (value != EE_NOT_FOUND) {
        return (int16_t)value;
    }

    return defValue;
}

void settingsStore(Param param, int16_t value)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return;
    }

    eeUpdateRaw(eeMap[param], (uint16_t)value);
}

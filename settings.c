#include "settings.h"

#include "audio/audiodefs.h"
#include "display/glcd.h"
#include "eemul.h"
#include "rc.h"
#include "screen.h"
#include "spectrum.h"
#include "tr/labels.h"
#include "tuner/tunerdefs.h"

static uint8_t silenceTimer = 0;
static int16_t rtcCorr = 0;
static uint8_t muteStby = 0;

#define GENERATE_EE_RC_MAP(CMD)  [PARAM_RC_ ## CMD] = {0x80 + RC_CMD_ ## CMD, (int16_t)EE_NOT_FOUND},

static const EE_Map eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_AUDIO_IC]        =   {0x01,  AUDIO_IC_TEST},
    [PARAM_AUDIO_INPUT]     =   {0x02,  0},
    [PARAM_AUDIO_LOUDNESS]  =   {0x03,  false},
    [PARAM_AUDIO_SURROUND]  =   {0x04,  false},
    [PARAM_AUDIO_EFFECT3D]  =   {0x05,  false},
    [PARAM_AUDIO_BYPASS]    =   {0x06,  false},

    [PARAM_AUDIO_IN0]       =   {0x10,  0},
    [PARAM_AUDIO_IN1]       =   {0x11,  1},
    [PARAM_AUDIO_IN2]       =   {0x12,  2},
    [PARAM_AUDIO_IN3]       =   {0x13,  3},
    [PARAM_AUDIO_IN4]       =   {0x14,  4},
    [PARAM_AUDIO_IN5]       =   {0x15,  5},
    [PARAM_AUDIO_IN6]       =   {0x16,  6},
    [PARAM_AUDIO_IN7]       =   {0x17,  7},

    [PARAM_AUDIO_GAIN0]     =   {0x18,  0},
    [PARAM_AUDIO_GAIN1]     =   {0x19,  0},
    [PARAM_AUDIO_GAIN2]     =   {0x1A,  0},
    [PARAM_AUDIO_GAIN3]     =   {0x1B,  0},
    [PARAM_AUDIO_GAIN4]     =   {0x1C,  0},
    [PARAM_AUDIO_GAIN5]     =   {0x1D,  0},
    [PARAM_AUDIO_GAIN6]     =   {0x1E,  0},
    [PARAM_AUDIO_GAIN7]     =   {0x1F,  0},

    [PARAM_AUDIO_VOLUME]    =   {0x20,  0},
    [PARAM_AUDIO_BASS]      =   {0x21,  0},
    [PARAM_AUDIO_MIDDLE]    =   {0x22,  0},
    [PARAM_AUDIO_TREBLE]    =   {0x23,  0},
    [PARAM_AUDIO_FRONTREAR] =   {0x24,  0},
    [PARAM_AUDIO_BALANCE]   =   {0x25,  0},
    [PARAM_AUDIO_CENTER]    =   {0x26,  0},
    [PARAM_AUDIO_SUBWOOFER] =   {0x27,  0},
    [PARAM_AUDIO_PREAMP]    =   {0x28,  0},

    [PARAM_TUNER_IC]        =   {0x30,  TUNER_IC_TEST},
    [PARAM_TUNER_BAND]      =   {0x31,  TUNER_BAND_FM_US_EUROPE},
    [PARAM_TUNER_STEP]      =   {0x32,  TUNER_STEP_100K},
    [PARAM_TUNER_DEEMPH]    =   {0x33,  TUNER_DEEMPH_50u},
    [PARAM_TUNER_MODE]      =   {0x34,  TUNER_MODE_GRID},
    [PARAM_TUNER_FMONO]     =   {0x35,  false},
    [PARAM_TUNER_RDS]       =   {0x36,  true},
    [PARAM_TUNER_BASS]      =   {0x37,  false},
    [PARAM_TUNER_VOLUME]    =   {0x38,  TUNER_VOLUME_MAX},
    [PARAM_TUNER_FREQ]      =   {0x39,  9950},

    [PARAM_DISPLAY_BR_STBY] =   {0x40,  3},
    [PARAM_DISPLAY_BR_WORK] =   {0x41,  LCD_BR_MAX},
    [PARAM_DISPLAY_ROTATE]  =   {0x42,  false},
    [PARAM_DISPLAY_DEF]     =   {0x43,  SCREEN_AUDIO_INPUT},
    [PARAM_DISPLAY_PALETTE] =   {0x44,  PAL_DEFAULT},

    [PARAM_SPECTRUM_MODE]   =   {0x50,  SP_MODE_STEREO},
    [PARAM_SPECTRUM_PEAKS]  =   {0x51,  true},

    [PARAM_ALARM_HOUR]      =   {0x60,  7},
    [PARAM_ALARM_MINUTE]    =   {0x61,  30},
    [PARAM_ALARM_DAYS]      =   {0x62,  0},

    [PARAM_SYSTEM_LANG]     =   {0x70,  LANG_DEFAULT},
    [PARAM_SYSTEM_MUTESTBY] =   {0x71,  MUTESTBY_POS},
    [PARAM_SYSTEM_SIL_TIM]  =   {0x72,  5},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  4},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

void settingsInit(void)
{
    eeInit();

    audioReadSettings();
    tunerReadSettings();

    settingsSet(PARAM_ALARM_HOUR, settingsRead(PARAM_ALARM_HOUR));
    settingsSet(PARAM_ALARM_MINUTE, settingsRead(PARAM_ALARM_MINUTE));
    settingsSet(PARAM_ALARM_DAYS, settingsRead(PARAM_ALARM_DAYS));

    settingsSet(PARAM_SYSTEM_MUTESTBY, settingsRead(PARAM_SYSTEM_MUTESTBY));
    settingsSet(PARAM_SYSTEM_SIL_TIM, settingsRead(PARAM_SYSTEM_SIL_TIM));
    settingsSet(PARAM_SYSTEM_RTC_CORR, settingsRead(PARAM_SYSTEM_RTC_CORR));
}

int16_t settingsGet(Param param)
{
    int16_t ret = 0;

    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {
    case PARAM_ALARM_HOUR:
        ret = alarm->hour;
        break;
    case PARAM_ALARM_MINUTE:
        ret = alarm->min;
        break;
    case PARAM_ALARM_DAYS:
        ret = (int16_t)alarm->days;
        break;
    case PARAM_SYSTEM_MUTESTBY:
        ret = muteStby;
        break;
    case PARAM_SYSTEM_SIL_TIM:
        ret = silenceTimer;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        ret = rtcCorr;
        break;
    default:
        break;
    }

    return  ret;
}

void settingsSet(Param param, int16_t value)
{
    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {
    case PARAM_ALARM_HOUR:
        alarm->hour = (int8_t)value;
        break;
    case PARAM_ALARM_MINUTE:
        alarm->min = (int8_t)value;
        break;
    case PARAM_ALARM_DAYS:
        alarm->days = (AlarmDay)value;
        break;
    case PARAM_SYSTEM_MUTESTBY:
        muteStby = (uint8_t)value;
        break;
    case PARAM_SYSTEM_SIL_TIM:
        silenceTimer = (uint8_t)value;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        rtcCorr = value;
        break;
    default:
        break;
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
    return eeMap[param].def;
}

void settingsStore(Param param, int16_t value)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return;
    }

    eeUpdateRaw(eeMap[param].cell, (uint16_t)value);
}

const EE_Map *eeMapGet(void)
{
    return eeMap;
}

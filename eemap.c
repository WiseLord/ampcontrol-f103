#include "eemap.h"

#include "audio/audiodefs.h"
#include "display/dispdefs.h"
#include "eemul.h"
#include "settings.h"
#include "spectrum.h"
#include "tr/labels.h"
#include "tuner/tunerdefs.h"

#define GENERATE_EE_RC_MAP(CMD)  {EE_RC_ ## CMD, (int16_t)EE_NOT_FOUND},

static const EE_Map eeMap[] = {
    {EE_NULL,                   0},

    {EE_AUDIO_IC,               AUDIO_IC_TEST},
    {EE_AUDIO_INPUT,            0},
    {EE_AUDIO_LOUDNESS,         false},
    {EE_AUDIO_SURROUND,         false},
    {EE_AUDIO_EFFECT3D,         false},
    {EE_AUDIO_BYPASS,           false},

    {EE_AUDIO_IN0,              0},
    {EE_AUDIO_IN1,              1},
    {EE_AUDIO_IN2,              2},
    {EE_AUDIO_IN3,              3},
    {EE_AUDIO_IN4,              4},
    {EE_AUDIO_IN5,              5},
    {EE_AUDIO_IN6,              6},
    {EE_AUDIO_IN7,              7},

    {EE_AUDIO_GAIN0,            0},
    {EE_AUDIO_GAIN1,            0},
    {EE_AUDIO_GAIN2,            0},
    {EE_AUDIO_GAIN3,            0},
    {EE_AUDIO_GAIN4,            0},
    {EE_AUDIO_GAIN5,            0},
    {EE_AUDIO_GAIN6,            0},
    {EE_AUDIO_GAIN7,            0},

    {EE_AUDIO_PARAM_VOLUME,     0},
    {EE_AUDIO_PARAM_BASS,       0},
    {EE_AUDIO_PARAM_MIDDLE,     0},
    {EE_AUDIO_PARAM_TREBLE,     0},
    {EE_AUDIO_PARAM_FRONTREAR,  0},
    {EE_AUDIO_PARAM_BALANCE,    0},
    {EE_AUDIO_PARAM_CENTER,     0},
    {EE_AUDIO_PARAM_SUBWOOFER,  0},
    {EE_AUDIO_PARAM_PREAMP,     0},

    {EE_TUNER_IC,               TUNER_IC_TEST},
    {EE_TUNER_BAND,             TUNER_BAND_FM_US_EUROPE},
    {EE_TUNER_STEP,             TUNER_STEP_100K},
    {EE_TUNER_DEEMPH,           TUNER_DEEMPH_50u},
    {EE_TUNER_MODE,             TUNER_MODE_GRID},
    {EE_TUNER_FMONO,            false},
    {EE_TUNER_RDS,              true},
    {EE_TUNER_BASS,             false},
    {EE_TUNER_VOLUME,           TUNER_VOLUME_MAX},
    {EE_TUNER_FREQ,             9950},

    {EE_DISPLAY_BR_STBY,        3},
    {EE_DISPLAY_BR_WORK,        LCD_BR_MAX},
    {EE_DISPLAY_ROTATE,         false},

    {EE_SPECTRUM_MODE,          SP_MODE_STEREO},
    {EE_SPECTRUM_PEAKS,         true},

    {EE_ALARM_HOUR,             7},
    {EE_ALARM_MINUTE,           30},
    {EE_ALARM_DAYS,             0},

    {EE_SYSTEM_ENC_RES,         4},
    {EE_SYSTEM_MUTESTBY,        MUTESTBY_POS},
    {EE_SYSTEM_SIL_TIM,         5},
    {EE_SYSTEM_RTC_CORR,        0},

    {EE_SYSTEM_LANG,            LANG_DEFAULT},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

uint16_t eeMapGetSize(void)
{
    return (sizeof(eeMap) / sizeof(eeMap[0]));
}

const EE_Map *eeMapGet(void)
{
    return  eeMap;
}

void eeUpdate(EE_Param param, int16_t value)
{
    eeUpdateRaw((uint16_t)param, (uint16_t)value);
}

int16_t eeRead(EE_Param param)
{
    uint16_t addr = (uint16_t)param;
    uint16_t value = eeReadRaw(addr);

    if (value != EE_NOT_FOUND) {
        return (int16_t)value;
    }

    // Return default value if not found
    int16_t def = (int16_t)EE_NOT_FOUND;

    const EE_Map *eeMap = eeMapGet();

    for (uint16_t i = 0; i < eeMapGetSize(); i++) {
        if (eeMap[i].par == addr) {
            def = eeMap[i].def;
            break;
        }
    }

    return def;
}

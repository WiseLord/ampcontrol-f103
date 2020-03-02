#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "cmd.h"

#define EE_EMPTY        (uint16_t)0xFFFF
#define EE_NOT_FOUND    (uint16_t)0x8000

#define GENERATE_PARAM_RC(CMD)  PARAM_RC_ ## CMD,

typedef struct {
    uint16_t cell;  // EE cell
    int16_t def;    // Default value
} EE_Map;

typedef uint8_t Param;
enum {
    PARAM_NULL = 0,

    //----------------------------------------------------------------

    PARAM_AUDIO_BEGIN,

    PARAM_AUDIO_IC = PARAM_AUDIO_BEGIN,
    PARAM_AUDIO_INPUT,
    PARAM_AUDIO_LOUDNESS,
    PARAM_AUDIO_SURROUND,
    PARAM_AUDIO_EFFECT3D,
    PARAM_AUDIO_BYPASS,

    PARAM_AUDIO_IN0,
    PARAM_AUDIO_IN1,
    PARAM_AUDIO_IN2,
    PARAM_AUDIO_IN3,
    PARAM_AUDIO_IN4,
    PARAM_AUDIO_IN5,
    PARAM_AUDIO_IN6,
    PARAM_AUDIO_IN7,
    PARAM_AUDIO_IN_LAST = PARAM_AUDIO_IN7,

    PARAM_AUDIO_GAIN0,
    PARAM_AUDIO_GAIN1,
    PARAM_AUDIO_GAIN2,
    PARAM_AUDIO_GAIN3,
    PARAM_AUDIO_GAIN4,
    PARAM_AUDIO_GAIN5,
    PARAM_AUDIO_GAIN6,
    PARAM_AUDIO_GAIN7,
    PARAM_AUDIO_GAIN_LAST = PARAM_AUDIO_GAIN7,

    PARAM_AUDIO_VOLUME,
    PARAM_AUDIO_BASS,
    PARAM_AUDIO_MIDDLE,
    PARAM_AUDIO_TREBLE,
    PARAM_AUDIO_FRONTREAR,
    PARAM_AUDIO_BALANCE,
    PARAM_AUDIO_CENTER,
    PARAM_AUDIO_SUBWOOFER,
    PARAM_AUDIO_PREAMP,

    PARAM_AUDIO_END,

    //----------------------------------------------------------------

    PARAM_TUNER_BEGIN = PARAM_AUDIO_END,

    PARAM_TUNER_IC = PARAM_TUNER_BEGIN,
    PARAM_TUNER_BAND,
    PARAM_TUNER_STEP,
    PARAM_TUNER_DEEMPH,
    PARAM_TUNER_STA_MODE,
    PARAM_TUNER_FMONO,
    PARAM_TUNER_RDS,
    PARAM_TUNER_BASS,
    PARAM_TUNER_VOLUME,
    PARAM_TUNER_FREQ,

    PARAM_TUNER_END,

    //----------------------------------------------------------------

    PARAM_DISPLAY_BEGIN = PARAM_TUNER_END,

    PARAM_DISPLAY_BR_STBY = PARAM_DISPLAY_BEGIN,
    PARAM_DISPLAY_BR_WORK,
    PARAM_DISPLAY_ROTATE,
    PARAM_DISPLAY_DEF,
    PARAM_DISPLAY_PALETTE,

    PARAM_SPECTRUM_MODE,
    PARAM_SPECTRUM_PEAKS,
    PARAM_SPECTRUM_GRAD,

    PARAM_ALARM_HOUR,
    PARAM_ALARM_MINUTE,
    PARAM_ALARM_DAYS,

    PARAM_SYSTEM_LANG,
    PARAM_SYSTEM_MUTESTBY,
    PARAM_SYSTEM_ENC_RES,
    PARAM_SYSTEM_SIL_TIM,
    PARAM_SYSTEM_RTC_CORR,

    PARAM_I2C_EXT_IN_STAT,
    PARAM_I2C_EXT_BT,

    FOREACH_CMD(GENERATE_PARAM_RC)

    PARAM_END
};

void settingsInit(void);

int16_t settingsGet(Param param);
void settingsSet(Param param, int16_t value);

int16_t settingsRead(Param param);
void settingsStore(Param param, int16_t value);

const EE_Map *eeMapGet(void);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H

#ifndef EEEMUL_H
#define EEEMUL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rc.h"

#define EE_PAGE_STEP    2
#define EE_PAGE_SIZE    1024

#define EE_EMPTY        (uint16_t)0xFFFF
#define EE_NOT_FOUND    (uint16_t)0x8000

#define GENERATE_EE_RC(CMD)  EE_RC_ ## CMD,

enum {
    EE_PAGE_0 = 124,
    EE_PAGE_1 = 126,
    EE_PAGE_END
};

typedef enum {
    EE_NULL = 0,

    EE_AUDIO_IC,
    EE_AUDIO_INPUT,
    EE_AUDIO_LOUDNESS,
    EE_AUDIO_SURROUND,
    EE_AUDIO_EFFECT3D,
    EE_AUDIO_BYPASS,

    EE_AUDIO_IN0 = 0x10,
    EE_AUDIO_IN1,
    EE_AUDIO_IN2,
    EE_AUDIO_IN3,
    EE_AUDIO_IN4,
    EE_AUDIO_IN5,
    EE_AUDIO_IN6,
    EE_AUDIO_IN7,

    EE_AUDIO_GAIN0 = 0x18,
    EE_AUDIO_GAIN1,
    EE_AUDIO_GAIN2,
    EE_AUDIO_GAIN3,
    EE_AUDIO_GAIN4,
    EE_AUDIO_GAIN5,
    EE_AUDIO_GAIN6,
    EE_AUDIO_GAIN7,

    EE_AUDIO_PARAM_VOLUME = 0x20,
    EE_AUDIO_PARAM_BASS,
    EE_AUDIO_PARAM_MIDDLE,
    EE_AUDIO_PARAM_TREBLE,
    EE_AUDIO_PARAM_FRONTREAR,
    EE_AUDIO_PARAM_BALANCE,
    EE_AUDIO_PARAM_CENTER,
    EE_AUDIO_PARAM_SUBWOOFER,
    EE_AUDIO_PARAM_PREAMP,

    EE_TUNER_IC = 0x30,
    EE_TUNER_BAND,
    EE_TUNER_STEP,
    EE_TUNER_DEEMPH,
    EE_TUNER_MODE,
    EE_TUNER_FMONO,
    EE_TUNER_RDS,
    EE_TUNER_BASS,
    EE_TUNER_VOLUME,
    EE_TUNER_FREQ,

    EE_DISPLAY_BR_STBY = 0x40,
    EE_DISPLAY_BR_WORK,
    EE_DISPLAY_ROTATE,

    EE_SPECTRUM_MODE,
    EE_INPUT_ENC_RES,

    EE_LANGUAGE = 0x80,

    FOREACH_CMD(GENERATE_EE_RC)
} EE_Param;

void eeInit(void);

void eeUpdate(EE_Param param, int16_t data);
int16_t eeRead(EE_Param param);

#ifdef __cplusplus
}
#endif

#endif // EEEMUL_H

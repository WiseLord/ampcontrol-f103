#ifndef EEEMUL_H
#define EEEMUL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rc.h"

#define EE_PAGE_STEP    4
#define EE_PAGE_SIZE    1024

#define EE_EMPTY        (uint16_t)0xFFFF

#define GENERATE_EE_RC(CMD)  EE_RC_ ## CMD,

enum {
    EE_PAGE_0 = 120,
    EE_PAGE_1 = 124,
    EE_PAGE_END
};

typedef enum {
    EE_AUDIO_IC,
    EE_AUDIO_INPUT,
    EE_AUDIO_FLAG,
    EE_AUDIO_PARAM_VOLUME,
    EE_AUDIO_PARAM_BASS,
    EE_AUDIO_PARAM_MIDDLE,
    EE_AUDIO_PARAM_TREBLE,
    EE_AUDIO_PARAM_FRONTREAR,
    EE_AUDIO_PARAM_BALANCE,
    EE_AUDIO_PARAM_CENTER,
    EE_AUDIO_PARAM_SUBWOOFER,
    EE_AUDIO_PARAM_PREAMP,

    EE_AUDIO_GAIN0,
    EE_AUDIO_GAIN1,
    EE_AUDIO_GAIN2,
    EE_AUDIO_GAIN3,
    EE_AUDIO_GAIN4,
    EE_AUDIO_GAIN5,
    EE_AUDIO_GAIN6,
    EE_AUDIO_GAIN7,

    EE_BRIGHTNESS_STBY,
    EE_BRIGHTNESS_WORK,

    EE_TUNER_IC,
    EE_TUNER_FREQ,
    EE_TUNER_FLAGS,
    EE_TUNER_BAND,
    EE_TUNER_STEP,
    EE_TUNER_DEEMPH,
    EE_TUNER_VOLUME,

    EE_LANGUAGE = 128, // Skip half of items to fix RC offset
    FOREACH_CMD(GENERATE_EE_RC)

    EE_PARAM_END
} EE_Param;

void eeInit(void);

void eeUpdate(EE_Param param, int16_t data);
uint16_t eeRead(EE_Param param);

uint16_t eeReadU(EE_Param param, uint16_t def);
int16_t eeReadI(EE_Param param, int16_t def);

#ifdef __cplusplus
}
#endif

#endif // EEEMUL_H

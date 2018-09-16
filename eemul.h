#ifndef EEEMUL_H
#define EEEMUL_H

#include <stm32f103xb.h>

#define EE_PAGE_STEP    2
#define EE_PAGE_SIZE    1024

#define EE_EMPTY        (uint16_t)0xFFFF

enum {
    EE_PAGE_0 = 60,
    EE_PAGE_1 = 62,
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

    EE_PARAM_END
} EE_Param;

void eeInit(void);

void eeUpdate(EE_Param param, int16_t data);
uint16_t eeRead(EE_Param param);

#endif // EEEMUL_H

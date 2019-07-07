#include "tda7440.h"

#include <stddef.h>

#include "../i2c.h"
#include "../pins.h"

inline void tda7440Init(AudioParam *param)
{
    tda7439Init(param);
    param->item[AUDIO_TUNE_MIDDLE].grid  = NULL;
}

inline void tda7440SetTune(AudioTune tune, int8_t value)
{
    tda7439SetTune(tune, value);
}

inline void tda7440SetInput(uint8_t value)
{
    tda7439SetInput(value);
}

inline void tda7440SetMute(bool value)
{
    tda7439SetMute(value);
}

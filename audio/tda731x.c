#include "tda731x.h"

#include "../i2c.h"
#include "../pins.h"




static AudioParam *aPar;

void tda731xInit(AudioParam *param, AudioIC ic)
{
    aPar = param;

}

void tda731xSetTune(AudioTune tune, int8_t value)
{

}

void tda731xSetInput(uint8_t value)
{

}

void tda731xSetMute(bool value)
{

}

void tda731xSetLoudness(bool value)
{

}

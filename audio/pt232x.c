#include "pt232x.h"

#include "../i2c.h"
#include "../pins.h"




static AudioParam *aPar;

void pt232xInit(AudioParam *param)
{
    aPar = param;

}

void pt232xSetTune(AudioTune tune, int8_t value)
{

}

void pt232xSetInput(uint8_t value)
{

}

void pt232xSetMute(bool value)
{

}

#include <stdlib.h>
#include <time.h>

#include "../../../eemul.h"
#include "../../../spectrum.h"

static Spectrum spectrum;
static RcData rcData;

uint16_t eeReadU(EE_Param param, uint16_t def)
{
    switch (param) {
    default:
        return def;
    }
}

int16_t eeReadI(EE_Param param, int16_t def)
{
    switch (param) {
    case EE_AUDIO_IN0:
        return 1;
    case EE_AUDIO_PARAM_VOLUME:
        return -30;
    case EE_INPUT_ENC_RES:
        return 0;
    default:
        return def;
    }
}

bool eeReadB(EE_Param param, bool def)
{
    switch (param) {
    default:
        return def;
    }
}

void eeUpdate(EE_Param param, int16_t data)
{
    (void)param;
    (void)data;
}

uint16_t rcGetCode(RcCmd cmd)
{
    (void)cmd;
    return 0;
}

void rcSaveCode(uint16_t cmd, uint16_t value)
{
    (void)cmd, (void)value;
}

RcData rcRead(bool clear)
{
    (void)clear;
    return rcData;
}

RcCmd rcGetCmd(RcData *rcData)
{
    (void) rcData;
    return RC_CMD_END;
}

Spectrum *spGet(void)
{
    return &spectrum;
}

void spGetADC(uint8_t *dataL, uint8_t *dataR)
{
    for (uint8_t i = 0; i < SPECTRUM_SIZE; i++) {
        dataL[i] = rand() & 0xFF;
        dataR[i] = rand() & 0xFF;
    }
}

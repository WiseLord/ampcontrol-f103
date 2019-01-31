#include <stdlib.h>
#include <time.h>

#include "../../../audio/audiodefs.h"
#include "../../../tuner/tunerdefs.h"
#include "../../../eemap.h"
#include "../../../spectrum.h"

static Spectrum spectrum;
static RcData rcData;

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

void pinsSetMuteStby(bool value)
{
    (void)value;
}

void pinsSetMute(bool value)
{
    (void)value;
}

void pinsSetStby(bool value)
{
    (void)value;
}

bool pinsGetMuteStby(void)
{
    return true;
}

void pinsDeInitAmpI2c(void)
{
}

void pinsInitAmpI2c(void)
{
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

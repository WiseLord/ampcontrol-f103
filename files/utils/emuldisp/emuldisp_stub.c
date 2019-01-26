#include <stdlib.h>
#include <time.h>

#include "../../../eemap.h"
#include "../../../spectrum.h"

static Spectrum spectrum;
static RcData rcData;

void eeUpdateRaw(uint16_t addr, uint16_t data)
{
    (void)addr;
    (void)data;
}

uint16_t eeReadRaw(uint16_t addr)
{
    (void)addr;

    return EE_NOT_FOUND;
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

#ifndef TUNER_H
#define TUNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

typedef struct {
    const TunerApi *api;
    TunerParam par;
    TunerStatus status;
} Tuner;

void tunerReadSettings(TunerIC defIc);
void tunerSaveSettings(void);

void tunerInit(void);
Tuner *tunerGet(void);

void tunerSetPower(bool value);

void tunerSetFreq(uint16_t value);

void tunerSetMute(bool value);
void tunerSetBassBoost(bool value);
void tunerSetForcedMono(bool value);
void tunerSetRds(bool value);

void tunerSetVolume(int8_t value);

void tunerSeek(int8_t direction);
void tunerStep(int8_t direction);
void tunerMove(int8_t direction);

void tunerUpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // TUNER_H

#ifndef TUNER_H
#define TUNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

typedef struct {
    void (*init)(TunerParam *param, TunerStatus *status);
    void (*setFreq)(uint16_t freq);
    void (*seek)(int8_t direction);

    void (*setVolume)(int8_t value);

    void (*setMute)(bool value);
    void (*setBassBoost)(bool value);
    void (*setForcedMono)(bool value);
    void (*setRds)(bool value);

    void (*setPower)(bool value);

    void (*updateStatus)(void);
} TunerApi;

typedef struct {
    TunerApi api;
    TunerParam par;
    TunerStatus status;
} Tuner;

void tunerReadSettings(void);
void tunerSaveSettings(void);

void tunerInit(void);
Tuner *tunerGet(void);

void tunerSetPower(bool value);

void tunerSetFreq(uint16_t value);

void tunerSetMute(bool value);
void tunerSetBassBoost(bool value);
void tunerSetForcedMono(bool value);
void tunerSetRds(bool value);

void tunerNextStation(int8_t direction);

void tunerUpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // TUNER_H

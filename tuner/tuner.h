#ifndef TUNER_H
#define TUNER_H

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

typedef enum {
    TUNER_IC_NO = 0,
    TUNER_IC_RDA5807,
    TUNER_IC_SI4703,
    TUNER_IC_TEA5767,

    TUNER_IC_END
} TunerIC;

typedef struct {
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
    TunerIC ic;
    TunerApi api;
    TunerParam par;
    TunerStatus status;
} Tuner;

void tunerInit(void);
Tuner *tunerGet(void);

void tunerSetPower(bool value);

void tunerSetFreq(uint16_t value);
void tunerSetFlag(TunerFlag flag, bool value);

void tunerNextStation(int8_t direction);

void tunerUpdateStatus(void);

#endif // TUNER_H

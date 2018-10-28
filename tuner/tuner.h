#ifndef TUNER_H
#define TUNER_H

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

#ifdef _RDA580X
#include "rda580x.h"
#endif
#ifdef _SI470X
#include "si470x.h"
#endif

typedef enum {
    TUNER_IC_NO = 0,
    TUNER_IC_RDA5807,
    TUNER_IC_SI4703,

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

    uint16_t (*getFreq)(void);
    uint8_t (*getRssi)(void);   // Signal level
} TunerApi;

typedef struct {
    TunerIC ic;
    TunerApi api;
    TunerParam par;

    uint16_t freq;
} Tuner;

void tunerInit(void);
Tuner *tunerGet(void);
TunerParam *tunerGetPar(void);

void tunerSetPower(bool value);

void tunerSetFreq(uint16_t value);
void tunerSetFlag(TunerFlag flag, bool value);

void tunerNextStation(int8_t direction);

void tunerUpdateStatus(void);

#endif // TUNER_H

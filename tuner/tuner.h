#ifndef TUNER_H
#define TUNER_H

#include <stdint.h>

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

    void (*setMute)(uint8_t value);
    void (*setBassBoost)(uint8_t value);
    void (*setForcedMono)(uint8_t value);
    void (*setRds)(uint8_t value);

    void (*setPower)(uint8_t value);

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

void tunerSetPower(uint16_t value);

void tunerSetFreq(uint16_t value);
void tunerSetFlag(TunerFlag flag, uint8_t value);

void tunerNextStation(int8_t direction);

void tunerUpdateStatus(void);

#endif // TUNER_H

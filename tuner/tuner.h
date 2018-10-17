#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

#ifdef _RDA580X
#include "rda580x.h"
#endif
#ifdef _SI470X
#include "si470x.h"
#endif

#define TUNER_DIR_UP        (1)
#define TUNER_DIR_DOWN      (-1)

typedef enum {
    TUNER_IC_NO = 0,

    TUNER_IC_RDA5807,
    TUNER_IC_SI4703,

    TUNER_IC_END
} TunerIC;

typedef enum {
    TUNER_FLAG_INIT     = 0x0000,

    TUNER_FLAG_MUTE     = 0x0001,
    TUNER_FLAG_BASS     = 0x0002,

    TUNER_FLAG_STEREO   = 0x0010,
    TUNER_FLAG_RDS      = 0x0020,
} TunerFlag;

typedef struct {
    void (*setFreq)(uint16_t freq);
    void (*seek)(int8_t direction);

    void (*setMute)(uint8_t value);
    void (*setBass)(uint8_t value);
    void (*setStereo)(uint8_t value);
    void (*setRds)(uint8_t value);

    void (*setPower)(uint8_t value);

    void (*setVolume)(int8_t value);

    void (*updateStatus)(void);

    uint16_t (*getFreq)(void);  // Actual frequency
    uint8_t (*getRssi)(void);   // Signal level
} TunerDriverApi;

typedef struct {
    TunerDriverApi api;
    TunerIC ic;

    TunerFlag flags;

    uint16_t freq;
    uint16_t freqMin;
    uint16_t freqMax;
} Tuner;

void tunerInit(void);
Tuner *tunerGet(void);

void tunerSetPower(uint16_t value);

void tunerSetFreq(uint16_t value);
void tunerSetFlag(TunerFlag flag, uint8_t value);

void tunerNextStation(int8_t direction);

void tunerUpdateStatus(void);

#endif // TUNER_H

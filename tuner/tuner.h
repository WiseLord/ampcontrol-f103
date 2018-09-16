#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

typedef enum {
    TUNER_IC_NO = 0,

    TUNER_IC_RDA5807,
    TUNER_IC_SI4703,

    TUNER_IC_END
} TunerIC;

typedef enum {
    TUNER_FLAG_INIT     = 0x0000,

    TUNER_FLAG_MUTE     = 0x0001,

    TUNER_FLAG_STEREO   = 0x0010,
    TUNER_FLAG_RDS      = 0x0020,

    TUNER_FLAG_BASS     = 0x0100,
} TunerFlag;

typedef struct {
    void (*setFlag)(void);
    void (*setFreq)(void);
    TunerIC ic;
    TunerFlag flag;
    uint16_t freq;
    uint16_t freqMin;
    uint16_t freqMax;
    uint16_t freqRead;
} Tuner;

void tunerInit(void);
Tuner *tunerGet(void);

void tunerSetFreq(uint16_t value);
void tunerSetFlag(TunerFlag flag, uint8_t value);

#endif // TUNER_H

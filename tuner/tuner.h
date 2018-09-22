#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

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
} Tuner;

void tunerInit(void);
Tuner *tunerGet(void);

void tunerPowerOn(void);
void tunerPowerOff(void);

void tunerSetFreq(uint16_t value);
void tunerSetFlag(TunerFlag flag, uint8_t value);

void tunerNextStation(int8_t direction);

#endif // TUNER_H

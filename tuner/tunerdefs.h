#ifndef TUNERDEFS_H
#define TUNERDEFS_H

#include <stdint.h>

#define TUNER_DIR_UP        (1)
#define TUNER_DIR_DOWN      (-1)

#define TUNER_VOLUME_MIN    0
#define TUNER_VOLUME_MAX    15

typedef enum {
    TUNER_FLAG_INIT     = 0x0000,

    TUNER_FLAG_MUTE     = 0x0001,
    TUNER_FLAG_BASS     = 0x0002,

    TUNER_FLAG_MONO     = 0x0010,
    TUNER_FLAG_RDS      = 0x0020,
} TunerFlag;

typedef enum {
    TUNER_BAND_FM_US_EUROPE,    // 87..108 MHz
    TUNER_BAND_FM_JAPAN,        // 76..97 MHz
    TUNER_BAND_FM_WORLDWIDE,    // 79..108 MHz
    TUNER_BAND_FM_EASTEUROPE,   // 65..76 MHz

    TUNER_BAND_END,
} TunerBand;

typedef enum {
    TUNER_STEP_50K,
    TUNER_STEP_100K,
    TUNER_STEP_200K,

    TUNER_STEP_END,
} TunerStep;

typedef enum {
    TUNER_DEEMPH_50u,
    TUNER_DEEMPH_75u,

    TUNER_DEEMPH_END,
} TunerDeemph;

typedef enum {
    TUNER_FREQ_CURR,
    TUNER_FREQ_MIN,
    TUNER_FREQ_MAX,

    TUNER_FREQ_END,
} TunerFreq;

typedef struct {
    TunerFlag flags;
    TunerBand band;
    TunerStep step;
    TunerDeemph deemph;
    uint8_t volume;

    uint16_t fMin;
    uint16_t fMax;
    uint8_t fStep;
} TunerParam;

#endif // TUNERDEFS_H

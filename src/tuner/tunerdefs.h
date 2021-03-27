#ifndef TUNERDEFS_H
#define TUNERDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define TUNER_DIR_UP        (1)
#define TUNER_DIR_DOWN      (-1)

#define TUNER_VOLUME_MIN    0
#define TUNER_VOLUME_MAX    15

#define FOREACH_TUNER_IC(TUNER_IC)  \
    TUNER_IC(NO)                    \
    TUNER_IC(RDA5807)               \
    TUNER_IC(SI4703)                \
    TUNER_IC(TEA5767)               \
    TUNER_IC(LC7213X)               \
    TUNER_IC(TEST)                  \

#define GENERATE_TUNER_IC(IC) TUNER_IC_ ## IC,

typedef uint8_t TunerIC;
enum {
    FOREACH_TUNER_IC(GENERATE_TUNER_IC)

    TUNER_IC_END
};

typedef uint8_t TunerStatusFlag;
enum {
    TUNER_STATUS_INIT       = 0x00,

    TUNER_STATUS_READY      = 0x01, // Ready (seek/tune complete)
    TUNER_STATUS_STEREO     = 0x02, // Stereo reception
    TUNER_STATUS_BANDLIM    = 0x04, // Band limit reached
    TUNER_STATUS_STATION    = 0x08, // Tuned at station

    TUNER_STATUS_SEEKUP     = 0x10, // Seek up in progress
    TUNER_STATUS_SEEKDOWN   = 0x20, // Seek down in progress
};

typedef uint8_t TunerParamFlag;
enum {
    TUNER_PARAM_INIT        = 0x00,

    TUNER_PARAM_MUTE        = 0x01,
    TUNER_PARAM_BASS        = 0x02,
    TUNER_PARAM_MONO        = 0x04,
    TUNER_PARAM_RDS         = 0x08,

    TUNER_PARAM_STA_MODE    = 0x10,
};

typedef uint8_t TunerBand;
enum {
    TUNER_BAND_FM_US_EUROPE,  // 87..108 MHz
    TUNER_BAND_FM_JAPAN,      // 76..97 MHz
    TUNER_BAND_FM_WORLDWIDE,  // 79..108 MHz
    TUNER_BAND_FM_EASTEUROPE, // 65..76 MHz

    TUNER_BAND_END,
};

typedef uint8_t TunerStep;
enum {
    TUNER_STEP_50K,
    TUNER_STEP_100K,
    TUNER_STEP_200K,

    TUNER_STEP_END,
};

typedef uint8_t TunerDeemph;
enum {
    TUNER_DEEMPH_50u,
    TUNER_DEEMPH_75u,

    TUNER_DEEMPH_END,
};

typedef struct {
    TunerIC ic;
    TunerBand band;
    TunerStep step;
    TunerDeemph deemph;

    uint16_t fMin;
    uint16_t fMax;

    TunerParamFlag flags;
    uint8_t fStep;
    int8_t volume;
} TunerParam;

typedef struct {
    uint16_t freq;
    TunerStatusFlag flags;
    uint8_t rssi;
} TunerStatus;

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

#ifdef __cplusplus
}
#endif

#endif // TUNERDEFS_H

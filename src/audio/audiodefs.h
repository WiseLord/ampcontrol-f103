#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define MAX_INPUTS  8

#define FOREACH_AUDIO_IC(AUDIO_IC)  \
    AUDIO_IC(NO)                    \
    AUDIO_IC(TDA7439)               \
    AUDIO_IC(TDA7313)               \
    AUDIO_IC(PT232X)                \
    AUDIO_IC(TDA7418)               \
    AUDIO_IC(TDA7440)               \
    AUDIO_IC(TDA7719)               \
    AUDIO_IC(TEST)                  \

#define GENERATE_AUDIO_IC(IC)    AUDIO_IC_ ## IC,

#define FOREACH_AUDIO_TUNE(AUDIO_TUNE)  \
    AUDIO_TUNE(VOLUME)                  \
    AUDIO_TUNE(BASS)                    \
    AUDIO_TUNE(MIDDLE)                  \
    AUDIO_TUNE(TREBLE)                  \
    AUDIO_TUNE(BALANCE)                 \
    AUDIO_TUNE(FRONTREAR)               \
    AUDIO_TUNE(CENTER)                  \
    AUDIO_TUNE(SUBWOOFER)               \
    AUDIO_TUNE(PREAMP)                  \
    AUDIO_TUNE(LOUDNESS)                \
    AUDIO_TUNE(BASS_FREQ)               \
    AUDIO_TUNE(BASS_QUAL)               \
    AUDIO_TUNE(MIDDLE_KFREQ)            \
    AUDIO_TUNE(MIDDLE_QUAL)             \
    AUDIO_TUNE(TREBLE_KFREQ)            \
    AUDIO_TUNE(SUB_CUT_FREQ)            \
    AUDIO_TUNE(LOUD_PEAK_FREQ)          \
    AUDIO_TUNE(GAIN)                    \

#define GENERATE_AUDIO_TUNE(TUNE)    AUDIO_TUNE_ ## TUNE,

typedef uint8_t AudioIC;
enum {
    FOREACH_AUDIO_IC(GENERATE_AUDIO_IC)

    AUDIO_IC_END
};

typedef int8_t AudioTune;
enum {
    AUDIO_TUNE_INVALID = -1,

    FOREACH_AUDIO_TUNE(GENERATE_AUDIO_TUNE)

    AUDIO_TUNE_END,
};

typedef uint8_t AudioFlag;
enum {
    AUDIO_FLAG_MUTE     = 0x01,
    AUDIO_FLAG_LOUDNESS = 0x02,
    AUDIO_FLAG_SURROUND = 0x04,
    AUDIO_FLAG_EFFECT3D = 0x08,
    AUDIO_FLAG_BYPASS   = 0x10,
};

typedef uint8_t AudioMode;
enum {
    AUDIO_MODE_2_0 = 0,
    AUDIO_MODE_2_1,
    AUDIO_MODE_4_0,
    AUDIO_MODE_4_1,
    AUDIO_MODE_5_1,

    AUDIO_MODE_END
};

typedef int8_t AudioGroup;
enum {
    AUDIO_GROUP_INVALID = -1,

    AUDIO_GROUP_VOLUME = 0,
    AUDIO_GROUP_BASS,
    AUDIO_GROUP_MIDDLE,
    AUDIO_GROUP_TREBLE,
    AUDIO_GROUP_BALANCE,
    AUDIO_GROUP_SUBFOOWER,
    AUDIO_GROUP_LOUDNESS,

    AUDIO_GROUP_END,
};

typedef struct {
    int8_t frontLeft;
    int8_t frontRight;
    int8_t rearLeft;
    int8_t rearRight;
    int8_t center;
    int8_t subwoofer;
} AudioRaw;

#define STEP_MULT   8

typedef struct {
    const int16_t *array;
    int8_t min;     // Minimum in steps
    int8_t max;     // Maximum in steps
    int8_t mStep;   // Step multiplied by STEP_MULT (to handle 1.25dB real step)
} AudioGrid;

typedef struct {
    AudioIC ic;
    int8_t input;
    AudioMode mode;
    AudioFlag flags;
    int8_t inCfg;
    int8_t gain[MAX_INPUTS];
    int8_t tune[AUDIO_TUNE_END];
    const AudioGrid *grid[AUDIO_TUNE_END];
} AudioParam;

typedef struct {
    void (*init)(AudioParam *param);
    int8_t (*getInCnt)(void);

    void (*setTune)(AudioTune tune, int8_t value);
    void (*setInput)(int8_t value);

    void (*setMute)(bool value);
    void (*setLoudness)(bool value);
    void (*setSurround)(bool value);
    void (*setEffect3d)(bool value);
    void (*setBypass)(bool value);

    void (*setPower)(bool value);
} AudioApi;

#ifdef __cplusplus
}
#endif

#endif // AUDIODEFS_H

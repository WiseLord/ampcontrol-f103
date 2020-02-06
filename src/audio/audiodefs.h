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
    AUDIO_TUNE(FRONTREAR)               \
    AUDIO_TUNE(BALANCE)                 \
    AUDIO_TUNE(CENTER)                  \
    AUDIO_TUNE(SUBWOOFER)               \
    AUDIO_TUNE(PREAMP)                  \
    AUDIO_TUNE(GAIN)                    \

#define GENERATE_AUDIO_TUNE(TUNE)    AUDIO_TUNE_ ## TUNE,

typedef enum {
    FOREACH_AUDIO_IC(GENERATE_AUDIO_IC)

    AUDIO_IC_END
} AudioIC;

typedef uint8_t AudioTune;
enum {
    FOREACH_AUDIO_TUNE(GENERATE_AUDIO_TUNE)

    AUDIO_TUNE_END,

    AUDIO_FLAG_MUTE = 0,
    AUDIO_FLAG_LOUDNESS,
    AUDIO_FLAG_SURROUND,
    AUDIO_FLAG_EFFECT3D,
    AUDIO_FLAG_BYPASS,
};

typedef uint8_t InputType;
enum {
    IN_TUNER = 0,
    IN_PC,
    IN_TV,
    IN_BLUETOOTH,
    IN_DVD,
    IN_USB,
    IN_MICROPHONE,
    IN_GUITAR,
    IN_TURNTABLES,
    IN_CASSETTE,
    IN_PROJECTOR,
    IN_SATELLITE,
    IN_MIXER,
    IN_KARADIO,

    IN_END
};

typedef struct {
    int8_t min;     // Minimum in steps
    int8_t max;     // Maximum in steps
    uint8_t mStep;   // Step multiplied by STEP_MULT (to handle 1.25 and so on)
} AudioGrid;

typedef struct {
    const AudioGrid *grid;
    int8_t value;   // Value in steps
} AudioTuneItem;

typedef struct {
    AudioIC ic;
    AudioTuneItem tune[AUDIO_TUNE_END];

    InputType inType[MAX_INPUTS];
    int8_t gain[MAX_INPUTS];
    int8_t input;
    int8_t inCnt;

    bool mute;
    bool loudness;
    bool surround;
    bool effect3d;
    bool bypass;
} AudioParam;

typedef struct {
    void (*initParam)(AudioParam *param);
    void (*reset)(void);

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

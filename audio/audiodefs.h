#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define MAX_INPUTS  8

typedef enum {
    AUDIO_IC_NO = 0,
    AUDIO_IC_TDA7439,
    AUDIO_IC_TDA7313,
    AUDIO_IC_PT232X,
    AUDIO_IC_TDA7418,
    AUDIO_IC_TDA7440,

    AUDIO_IC_TEST,

    AUDIO_IC_END
} AudioIC;

typedef uint8_t AudioTune;
enum {
    AUDIO_TUNE_VOLUME = 0,
    AUDIO_TUNE_BASS,
    AUDIO_TUNE_MIDDLE,
    AUDIO_TUNE_TREBLE,
    AUDIO_TUNE_FRONTREAR,
    AUDIO_TUNE_BALANCE,
    AUDIO_TUNE_CENTER,
    AUDIO_TUNE_SUBWOOFER,
    AUDIO_TUNE_PREAMP,

    AUDIO_TUNE_GAIN,

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
    AudioTuneItem item[AUDIO_TUNE_END];

    InputType inType[MAX_INPUTS];
    int8_t gain[MAX_INPUTS];
    uint8_t input;
    uint8_t inCnt;

    bool mute;
    bool loudness;
    bool surround;
    bool effect3d;
    bool bypass;
} AudioParam;

#ifdef __cplusplus
}
#endif

#endif // AUDIODEFS_H

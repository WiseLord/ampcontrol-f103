#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_INPUTS  8

typedef enum {
    AUDIO_IC_NO = 0,
    AUDIO_IC_TDA7439,
    AUDIO_IC_TDA7313,
    AUDIO_IC_PT232X,

    AUDIO_IC_END
} AudioIC;

typedef enum {
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

    AUDIO_TUNE_END
} AudioTune;

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

    bool mute;
    bool loudness;
    bool surround;
    bool effect3d;
    bool bypass;

    int8_t gain[MAX_INPUTS];
    uint8_t input;
    uint8_t inCnt;
} AudioParam;

#endif // AUDIODEFS_H

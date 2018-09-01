#ifndef AUDIO_H
#define AUDIO_H

#include <inttypes.h>

#define STEP_MULT   8

typedef enum {
    AUDIO_PARAM_VOLUME = 0,
    AUDIO_PARAM_BASS,
    AUDIO_PARAM_MIDDLE,
    AUDIO_PARAM_TREBLE,
    AUDIO_PARAM_PREAMP,
    AUDIO_PARAM_FRONTREAR,
    AUDIO_PARAM_BALANCE,
    AUDIO_PARAM_CENTER,
    AUDIO_PARAM_SUBWOOFER,
    AUDIO_PARAM_GAIN0,
    AUDIO_PARAM_GAIN1,
    AUDIO_PARAM_GAIN2,
    AUDIO_PARAM_GAIN3,
    AUDIO_PARAM_GAIN4,

    AUDIO_PARAM_END
} AudioParam;

typedef enum {
    AUDIO_IC_NO = 0,

    AUDIO_IC_TDA7439,
    AUDIO_IC_TDA7313,
    AUDIO_IC_PT232X,

    AUDIO_IC_END
} AudioIC;

typedef enum {
    AUDIO_FLAG_MUTE     = 0x0001,

    AUDIO_FLAG_LOUDNESS = 0x0010,
    AUDIO_FLAG_SURROUND = 0x0020,
    AUDIO_FLAG_EFFECT3D = 0x0040,
    AUDIO_FLAG_BYPASS   = 0x0080,
} AudioFlag;

typedef struct {
    int16_t min;     // Minimum in steps
    int16_t max;     // Maximum in steps
    int16_t step;    // Step multiplied by STEP_MULT (to handle 1.25 and so on)
} AudioGrid;

typedef struct {
    void (*set)(void);
    const AudioGrid *grid;
    int8_t value;   // Value in steps
} AudioItem;

typedef struct {
    void (*setFlag)(void);
    AudioIC ic;
    AudioFlag flag;
    AudioItem item[AUDIO_PARAM_END];
    uint8_t input;
    uint8_t inCnt;
} AudioProc;

void audioInit(void);
AudioProc *audioProcGet(void);

void audioSetInput(uint8_t value);
void audioSetParam(AudioParam param, int8_t value);
void audioChangeParam(AudioParam param, int8_t diff);
void audioSetFlag(AudioFlag flag, uint8_t value);

#endif // AUDIO_H

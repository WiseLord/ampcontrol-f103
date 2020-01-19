#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define STEP_MULT   8

typedef struct {
    const AudioApi *api;
    AudioParam par;
    AudioTune tune;
} AudioProc;

void audioReadSettings(void);
void audioSaveSettings(void);

void audioInit(void);
AudioProc *audioGet(void);

void audioSetPower(bool value);

void audioSetTune(AudioTune tune, int8_t value);
void audioChangeTune(AudioTune tune, int8_t diff);

void audioSetInput(int8_t value);
void audioSetMute(bool value);
void audioSetLoudness(bool value);
void audioSetSurround(bool value);
void audioSetEffect3D(bool value);
void audioSetBypass(bool value);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H

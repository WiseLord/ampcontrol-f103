#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

typedef struct {
    const AudioApi *api;
    AudioParam par;
} AudioProc;

void audioReadSettings(AudioIC ic);
void audioSaveSettings(void);

void audioInit(void);

AudioProc *audioGet(void);

void audioSetRawBalance(AudioRaw *raw, int8_t volume, bool rear2bass);
void audioSetPower(bool value);

void audioSetTune(AudioTune tune, int8_t value);
void audioChangeTune(AudioTune tune, int8_t diff);

void audioSetInput(int8_t value);
int8_t audioGetInputCount(void);

void audioSetFlag(AudioFlag flag, bool value);

bool audioIsModeSupported(AudioMode mode);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H

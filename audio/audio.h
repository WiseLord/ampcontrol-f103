#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define STEP_MULT   8

typedef enum {
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

    IN_END
} InputType;

typedef struct {
    void (*init)(AudioParam *param);

    void (*setTune)(AudioTune tune, int8_t value);
    void (*setInput)(uint8_t value);

    void (*setMute)(bool value);
    void (*setLoudness)(bool value);
    void (*setSurround)(bool value);
    void (*setEffect3d)(bool value);
    void (*setBypass)(bool value);

    void (*setPower)(bool value);
} AudioApi;

typedef struct {
    AudioApi api;
    AudioParam par;
} AudioProc;

void audioReadSettings(void);
void audioSaveSettings(void);

void audioInit(void);
AudioProc *audioGet(void);

void audioSetPower(bool value);

void audioSetTune(AudioTune tune, int8_t value);
void audioChangeTune(AudioTune tune, int8_t diff);

void audioSetInput(uint8_t value);
void audioSetFlag(AudioFlag flag, bool value);


#endif // AUDIO_H

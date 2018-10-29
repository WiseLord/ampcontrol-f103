#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "audio/audio.h"
#include "rtc.h"

typedef enum {
    INPUT_TUNER = 0,

    INPUT_END
} Input;

typedef enum {
    SCREEN_STANDBY,

    SCREEN_TIME,
    SCREEN_SPECTRUM,
    SCREEN_BRIGHTNESS,
    SCREEN_AUDIO_INPUT,
    SCREEN_AUDIO_PARAM,

    SCREEN_TUNER,

    SCREEN_MENU,

    SCREEN_END
} Screen;

typedef union {
    AudioTune tune;
    uint8_t input;
    RtcMode rtc;
    uint8_t parent;
} ScreenParam;

void screenReadSettings(void);
void screenSaveSettings(void);

void screenInit(void);
void screenClear(void);
void screenUpdate(void);

void screenSet(Screen value);
Screen screenGet(void);

void screenSetParam(ScreenParam param);

void screenSetDefault(Screen value);
Screen screenGetDefault(void);

int8_t screenGetBrightness(uint8_t mode);
void screenSetBrightness(uint8_t mode, int8_t value);
void screenChangeBrighness(uint8_t mode, int8_t diff);

void screenShow(void);

void screenShowTime(void);
void screenShowSpectrum(void);
void screenShowBrightness(void);
void screenShowInput(void);
void screenShowAudioParam(void);
void screenShowTuner(void);
void screenShowMenu(void);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_H

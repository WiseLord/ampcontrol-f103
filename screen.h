#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "audio/audio.h"
#include "rtc.h"
#include "menu.h"

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

typedef enum {
    BR_STBY,
    BR_WORK,

    BR_END
} BrMode;

typedef union {
    AudioTune tune;
    uint8_t input;
    RtcMode rtc;
    MenuIdx parent;
} ScreenParam;

void screenReadSettings(void);
void screenSaveSettings(void);

void screenInit(void);

void screenSet(Screen value);
Screen screenGet(void);

void screenSetParam(ScreenParam param);

void screenSetDefault(Screen value);
Screen screenGetDefault(void);

int8_t screenGetBrightness(BrMode mode);
void screenSetBrightness(BrMode mode, int8_t value);
void screenChangeBrighness(BrMode mode, int8_t diff);

void screenShow(void);

void screenShowTime(bool clear);
void screenShowSpectrum(bool clear);
void screenShowBrightness(bool clear);
void screenShowInput(bool clear);
void screenShowAudioParam(bool clear);
void screenShowTuner(bool clear);
void screenShowMenu(void);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_H

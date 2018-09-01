#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "audio/audio.h"
#include "rtc.h"

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
    SCREEN_SPECTRUM,
    SCREEN_BRIGHTNESS,
    SCREEN_AUDIO_INPUT,
    SCREEN_AUDIO_PARAM,

    SCREEN_TEST,
    SCREEN_END
} Screen;

typedef union {
    AudioParam audio;
    RtcMode rtc;
} ScreenParam;

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

#endif // SCREEN_H

#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "audio/audio.h"
#include "menu.h"
#include "spectrum.h"
#include "rtc.h"

typedef uint8_t ScreenMode;
enum {
    SCREEN_STANDBY,

    SCREEN_TIME,
    SCREEN_SPECTRUM,

    SCREEN_AUDIO_PARAM,
    SCREEN_AUDIO_FLAG,

    SCREEN_TUNER,

    SCREEN_MENU,

    SCREEN_TEXTEDIT,

    SCREEN_STBY_TIMER,
    SCREEN_SILENCE_TIMER,

    SCREEN_END
};

typedef enum {
    BR_STBY,
    BR_WORK,

    BR_END
} BrMode;

typedef struct {
    ScreenMode mode;
    ScreenMode def;
    int8_t br[BR_END];
} Screen;

void screenReadSettings(void);
void screenSaveSettings(void);

void screenInit(void);

void screenSet(ScreenMode value);
ScreenMode screenGet(void);

void screenSetDefault(ScreenMode value);
ScreenMode screenGetDefault(void);

int8_t screenGetBrightness(BrMode mode);
void screenSetBrightness(BrMode mode, int8_t value);
void screenChangeBrighness(BrMode mode, int8_t diff);

void screenToClear(void);
void screenShow(bool clear);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_H

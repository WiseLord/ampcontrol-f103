#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "display/icons/icons.h"

typedef uint8_t ScreenMode;
enum {
    SCREEN_STANDBY = 0,

    SCREEN_TIME,
    SCREEN_SPECTRUM,

    SCREEN_AUDIO_PARAM,
    SCREEN_AUDIO_FLAG,
    SCREEN_AUDIO_INPUT,

    SCREEN_MENU,

    SCREEN_TEXTEDIT,

    SCREEN_STBY_TIMER,
    SCREEN_SILENCE_TIMER,

    SCREEN_END
};

typedef enum {
    BR_STBY = 0,
    BR_WORK,

    BR_END
} BrMode;

typedef struct {
    ScreenMode mode;
    ScreenMode def;
    Icon iconHint;
    int8_t br[BR_END];
} Screen;

void screenReadSettings(void);
void screenSaveSettings(void);

void screenInit(void);

Screen *screenGet(void);

void screenSetMode(ScreenMode value);
ScreenMode screenGetMode(void);

int8_t screenGetBrightness(BrMode mode);
void screenSetBrightness(BrMode mode, int8_t value);
void screenChangeBrighness(BrMode mode, int8_t diff);

void screenToClear(void);
void screenShow(bool clear);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_H

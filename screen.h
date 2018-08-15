#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "rtc.h"

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
    SCREEN_SPECTRUM,
    SCREEN_BRIGHTNESS,
} Screen;

void screenInit(void);
void screenClear(void);
void screenUpdate(void);

void screenSet(Screen value);
Screen screenGet(void);

int8_t screenGetBrightness(uint8_t mode);
void screenSetBrightness(uint8_t mode, int8_t value);
void screenChangeBrighness(uint8_t mode, int8_t diff);

void screenTime(RtcMode etm);
void screenSpectrum(void);
void screenBrightness(void);

#endif // SCREEN_H

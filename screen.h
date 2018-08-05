#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "display/gdfb.h"
#include "rtc.h"

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
    SCREEN_SPECTRUM,
    SCREEN_BRIGHTNESS,
} Screen;

void screenSet(Screen value);
Screen screenGet(void);
void screenClear(void);

void screenTime(RtcMode etm);
void screenSpectrum(uint8_t speed);
void screenBrightness(void);

#endif // SCREEN_H

#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "display/gdfb.h"
#include "rtc.h"

#define writeString(x)          gdWriteString(x)
// String buffer

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
    SCREEN_SPECTRUM,
    SCREEN_BRIGHTNESS,
} Screen;

void screenSet(Screen value);
Screen screenGet(void);

void screenTime(RtcMode etm);
void screenSpectrum(void);
void screenBrightness(void);

#endif // SCREEN_H

#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "display/gdfb.h"
#include "rtc.h"

#define writeString(x)          gdWriteString(x)
// String buffer
#define STR_BUFSIZE             20

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
    SCREEN_SPECTRUM,
} Screen;

void screenTime(RtcMode etm);
void screenSpectrum(void);

#endif // SCREEN_H

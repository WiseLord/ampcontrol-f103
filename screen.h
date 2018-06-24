#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "display/gdfb.h"
#include "rtc.h"

#define writeString(x)          gdWriteString(x)
// String buffer
#define STR_BUFSIZE             20

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_END
} TxtLabel;

void screenTime(RtcMode etm);

void screenNum(int16_t num);

#endif // SCREEN_H

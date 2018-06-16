#ifndef RTC_H
#define RTC_H

#include <inttypes.h>

typedef struct {
    int8_t hour;
    int8_t min;
    int8_t sec;
    int8_t date;
    int8_t month;
    int8_t year;
    int8_t wday;
    int8_t etm;
} RTC_type;

void rtcInit();

uint32_t rtcToSec(RTC_type *rtc);
void secToRtc(uint32_t time, RTC_type *rtc);

#endif // RTC_H

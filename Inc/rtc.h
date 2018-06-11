#ifndef RTC_H
#define RTC_H

#include <inttypes.h>

typedef struct {
    int8_t hour;
    int8_t min;
    int8_t sec;
    int8_t date;
    int8_t wday;
    int8_t month;
    int16_t year;
} RTC_type;

void rtcInit();
void rtcFromUnix(uint32_t time, RTC_type *rtc);
uint32_t rtcToUnix(RTC_type *rtc);

#endif // RTC_H

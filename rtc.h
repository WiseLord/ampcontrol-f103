#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ALARM_COUNT 1

typedef uint8_t RtcMode;
enum {
    RTC_HOUR,
    RTC_MIN,
    RTC_SEC,
    RTC_DATE,
    RTC_MONTH,
    RTC_YEAR,

    RTC_NOEDIT,
};

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

typedef enum {
    ALARM_DAY_OFF = 0,
    ALARM_DAY_WEEKDAYS,
    ALARM_DAY_ALL_DAYS,

    ALARM_DAY_END
} AlarmDay;

typedef struct {
    int8_t hour;
    int8_t min;
    AlarmDay days;
} Alarm;

void rtcInit(void);
void rtcIRQ(void);

void rtcGetTime(RTC_type *rtc);
void rtcSetTime(int8_t mode, int8_t value);
void rtcChangeTime(int8_t mode, int8_t diff);
void rtcEditTime(int8_t mode, int8_t value);

int8_t rtcGetMode(void);
void rtcSetMode(int8_t mode);
void rtcChangeMode(int8_t diff);

Alarm *rtcGetAlarm(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif // RTC_H

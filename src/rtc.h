#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
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
    RtcMode etm;
} RTC_type;

typedef int8_t AlarmDay;
enum {
    ALARM_DAY_OFF = 0,
    ALARM_DAY_WEEKDAYS,
    ALARM_DAY_ALL_DAYS,

    ALARM_DAY_END
};

typedef uint8_t AlarmMode;
enum {
    ALARM_HOUR,
    ALARM_MIN,
    ALARM_DAYS,
};

typedef struct {
    int8_t hour;
    int8_t min;
    AlarmDay days;
} Alarm;

typedef void (*RtcCb)(void);

void rtcInit(void);
void rtcSetCb(RtcCb cb);
void rtcSetCorrection(int16_t value);

void rtcGetTime(RTC_type *rtc);
void rtcSetTime(RtcMode mode, int8_t value);
void rtcChangeTime(RtcMode mode, int8_t diff);
void rtcEditTime(RtcMode mode, int8_t value);

void rtcSetRaw(uint32_t value);
uint32_t rtcGetRaw(void);

RtcMode rtcGetMode(void);
void rtcSetMode(RtcMode mode);
void rtcChangeMode(int8_t diff);

Alarm *rtcGetAlarm(uint8_t index);
bool rtcCheckAlarm(void);
void rtcChangeAlarm(AlarmMode alarmMode,  int8_t diff);

#ifdef __cplusplus
}
#endif

#endif // RTC_H

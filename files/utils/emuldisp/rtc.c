#include "../../../rtc.h"

#include <time.h>

static uint32_t rtcTime;
static int8_t rtcMode = RTC_NOEDIT;

static const RTC_type rtcMin = { 0,  0, 0,  1,  1,  1, 0, RTC_NOEDIT};
static const RTC_type rtcMax = {23, 59, 0, 31, 12, 99, 0, RTC_NOEDIT};

static Alarm alarm[ALARM_COUNT];

static int8_t rtcDaysInMonth(RTC_type *rtc)
{
    int8_t ret = rtc->month;

    if (ret == 2) {
        ret = rtc->year & 0x03;
        ret = (ret ? 28 : 29);
    } else {
        if (ret > 7)
            ret++;
        ret |= 30;
    }

    return ret;
}

static uint32_t rtcToSec(RTC_type *rtc)
{
    int8_t a;
    int8_t y;
    int8_t m;
    int64_t tm;

    a = ((rtc->month < 3) ? 1 : 0);
    y = rtc->year + 1 - a;
    m = rtc->month + (12 * a) - 3;

    tm = rtc->date - 1;
    tm += (153 * m + 2) / 5;
    tm += (1461 * y + 3) / 4;
    tm -= 306;

    tm *= 60 * 60 * 24;

    tm += (rtc->hour * 3600);
    tm += (rtc->min * 60);
    tm += (rtc->sec);

    return (uint32_t)tm;
}

static void secToRtc(uint32_t time, RTC_type *rtc)
{
    int8_t a;
    int8_t y;
    int8_t m;
    int64_t tm = time;

    rtc->sec = tm % 60;
    tm /= 60;
    rtc->min = tm % 60;
    tm /= 60;
    rtc->hour = tm % 24;
    tm /= 24;

    tm += 306;

    rtc->wday = (tm + 1) % 7;

    y = (int8_t)((tm * 4) / 1461);
    tm -= (1461 * y + 3) / 4;
    m = (int8_t)((tm * 5 + 2) / 153);
    tm -= (153 * m + 2) / 5;

    a = ((m < 10) ? 1 : 0);

    rtc->year = y - a;
    rtc->month = m + (12 * a) - 9;
    rtc->date = (int8_t)(tm + 1);
}

static void rtcUpdate(RTC_type *rtc, int8_t mode, int8_t value)
{
    int8_t *time = (int8_t *)rtc + mode;
    int8_t timeMax = *((const int8_t *)&rtcMax + mode);
    int8_t timeMin = *((const int8_t *)&rtcMin + mode);

    if (mode == RTC_DATE)
        timeMax = rtcDaysInMonth(rtc);

    if (value > timeMax)
        value = timeMin;
    if (value < timeMin)
        value = timeMax;

    *time = value;

    uint32_t newTime = rtcToSec(rtc);

    rtcTime = newTime;
}

static bool rtcIsAlarmDay(AlarmDay days, int8_t wday)
{
    bool ret = false;

    switch (days) {
    case ALARM_DAY_WEEKDAYS:
        ret = (wday != 0 && wday != 6); // Not Sunday and not Saturday
        break;
    case ALARM_DAY_ALL_DAYS:
        ret = true;
        break;
    }

    return ret;
}

void rtcSetCorrection(int16_t value)
{
    (void)value;
}

void rtcGetTime(RTC_type *rtc)
{
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    rtc->hour = (int8_t)lt->tm_hour;
    rtc->min = (int8_t)lt->tm_min;
    rtc->sec = (int8_t)lt->tm_sec;

    rtc->date = (int8_t)lt->tm_mday;
    rtc->month = (int8_t)lt->tm_mon;
    rtc->year = (int8_t)lt->tm_year - 100;

    rtc->wday = (int8_t)lt->tm_wday;
}

void rtcSetTime(int8_t mode, int8_t value)
{
    RTC_type rtc;
    secToRtc(rtcTime, &rtc);

    rtcUpdate(&rtc, mode, value);
}

void rtcChangeTime(int8_t mode, int8_t diff)
{
    RTC_type rtc;
    secToRtc(rtcTime, &rtc);

    int8_t value = *((int8_t *)&rtc + mode);
    value += diff;

    rtcUpdate(&rtc, mode, value);
}

void rtcEditTime(int8_t mode, int8_t digit)
{
    RTC_type rtc;
    secToRtc(rtcTime, &rtc);

    int8_t value = *((int8_t *)&rtc + mode);
    int8_t timeMax = *((const int8_t *)&rtcMax + mode);
    if (mode == RTC_DATE)
        timeMax = rtcDaysInMonth(&rtc);

    value %= 10;
    value *= 10;
    if (value > timeMax)
        value = 0;
    value += digit;

    rtcUpdate(&rtc, mode, value);
}

RtcMode rtcGetMode(void)
{
    return rtcMode;
}

void rtcSetMode(RtcMode mode)
{
    rtcMode = mode;
}

void rtcChangeMode(int8_t diff)
{
    rtcMode += diff;

    if (rtcMode < RTC_HOUR) {
        rtcMode = RTC_NOEDIT;
    }
    if (rtcMode > RTC_NOEDIT) {
        rtcMode = RTC_HOUR;
    }
}

Alarm *rtcGetAlarm(uint8_t index)
{
    if (index >= ALARM_COUNT) {
        index = 0;
    }

    return &alarm[index];
}

bool rtcCheckAlarm()
{
    RTC_type rtc;
    secToRtc(rtcTime, &rtc);

    uint8_t alarmN = 0;

    if (rtc.hour == alarm[alarmN].hour &&
        rtc.min == alarm[alarmN].min &&
        rtc.sec == 0 &&
        rtcIsAlarmDay(alarm[alarmN].days, rtc.wday)) {
        return true;
    }

    return false;
}

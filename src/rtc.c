#include "rtc.h"

#include "hwlibs.h"

#include "settings.h"
#include "swtimers.h"

typedef uint8_t RtcPhase;
enum {
    RTC_INIT_DISABLED = 0,
    RTC_INIT_LSE_ENABLED,
    RTC_INIT_READY,

    RTC_INIT_END
};
static RtcPhase rtcPhase = RTC_INIT_DISABLED;

static uint32_t rtcTime;
static RtcMode rtcMode = RTC_NOEDIT;

static const RTC_type rtcMin = { 0,  0, -1,  1,  1,  1, 0, RTC_NOEDIT};
static const RTC_type rtcMax = {23, 59, 60, 31, 12, 99, 0, RTC_NOEDIT};

static Alarm alarm[ALARM_COUNT];

static RtcCb rtcCb;

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

static void getRtc(RTC_type *rtc)
{
#ifdef STM32F3
    // TODO: Do it once a second in the interrupt handler
    RTC_type rtc_temp;
    rtc_temp.hour = (int8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
    rtc_temp.min = (int8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
    rtc_temp.sec = (int8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
    rtc_temp.date = (int8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
    rtc_temp.month = (int8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
    rtc_temp.year = (int8_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));

    rtcTime = rtcToSec(&rtc_temp);
#endif

    int8_t a;
    int8_t y;
    int8_t m;
    int64_t tm = rtcTime;

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

static void rtcUpdate(RTC_type *rtc, RtcMode mode, int8_t value)
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

    if (mode == RTC_SEC) {
        if (value > 30) {
            value = 60;
        } else {
            value = 0;
        }
    }

    *time = value;

#ifdef STM32F1
    rtcTime = rtcToSec(rtc);

    LL_RTC_TIME_SetCounter(RTC, rtcTime);
#endif
#ifdef STM32F3
    if (mode < RTC_NOEDIT) {
        LL_RTC_DisableWriteProtection(RTC);
        LL_RTC_EnterInitMode(RTC);
        switch (mode) {
        case RTC_HOUR:
            LL_RTC_TIME_SetHour(RTC, __LL_RTC_CONVERT_BIN2BCD((uint8_t)value));
            break;
        case RTC_MIN:
            LL_RTC_TIME_SetMinute(RTC, __LL_RTC_CONVERT_BIN2BCD((uint8_t)value));
            break;
        case RTC_SEC:
            LL_RTC_TIME_SetSecond(RTC, __LL_RTC_CONVERT_BIN2BCD((uint8_t)value));
            break;
        case RTC_DATE:
            LL_RTC_DATE_SetDay(RTC, __LL_RTC_CONVERT_BIN2BCD((uint8_t)value));
            break;
        case RTC_MONTH:
            LL_RTC_DATE_SetMonth(RTC, __LL_RTC_CONVERT_BIN2BCD((uint8_t)value));
            break;
        case RTC_YEAR:
            LL_RTC_DATE_SetYear(RTC, __LL_RTC_CONVERT_BIN2BCD((uint8_t)value));
            break;
        }
        LL_RTC_ExitInitMode(RTC);
        LL_RTC_EnableWriteProtection(RTC);
    }
#endif
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

void rtcInit(void)
{
    switch (rtcPhase) {
    case RTC_INIT_DISABLED:
        // Power interface clock enable
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
        // Backup interface clock enable
#ifdef STM32F1
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);
#endif
        // Enable access to the backup domain
        LL_PWR_EnableBkUpAccess();

        if (LL_RCC_IsEnabledRTC()) {
            rtcPhase = RTC_INIT_READY;
        } else {
            // Backup domain reset
            LL_RCC_ForceBackupDomainReset();
            LL_RCC_ReleaseBackupDomainReset();
#ifdef STM32F3
            LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
#endif
            LL_RCC_LSE_Enable();

            rtcPhase = RTC_INIT_LSE_ENABLED;
        }
        break;
    case RTC_INIT_LSE_ENABLED:
        if (LL_RCC_LSE_IsReady()) {
            LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
            LL_RCC_EnableRTC();

#ifdef STM32F1
            LL_RTC_InitTypeDef rtc_initstruct = {32765, LL_RTC_CALIB_OUTPUT_NONE};
#endif
#ifdef STM32F3
            LL_RTC_InitTypeDef rtc_initstruct = {LL_RTC_HOURFORMAT_24HOUR, 0, 32765};
#endif

            if (LL_RTC_DeInit(RTC) != SUCCESS) {
                break;
            }

            if (LL_RTC_Init(RTC, &rtc_initstruct) != SUCCESS) {
                break;
            }

            rtcSetCorrection(settingsGet(PARAM_SYSTEM_RTC_CORR));

            rtcPhase = RTC_INIT_READY;
        }
        break;
    }

    if (rtcPhase == RTC_INIT_READY) {
#ifdef STM32F1
        LL_RTC_EnableIT_SEC(RTC);
        NVIC_EnableIRQ (RTC_IRQn);
#endif
        swTimSet(SW_TIM_RTC_INIT, SW_TIM_OFF);
    } else {
        swTimSet(SW_TIM_RTC_INIT, 500);
    }
}

void rtcSetCb(RtcCb cb)
{
    rtcCb = cb;
}

void rtcSetCorrection(int16_t value)
{
#ifdef STM32F1
    if (LL_RTC_EnterInitMode(RTC) != ERROR) {
        LL_RTC_CAL_SetCoarseDigital(BKP, (uint32_t)(64 - value));
    }
    LL_RTC_ExitInitMode(RTC);
#endif
#ifdef STM32F3
    LL_RTC_DisableWriteProtection(RTC);
    while (LL_RTC_IsActiveFlag_RECALP(RTC));
    LL_RTC_CAL_SetMinus(RTC, (uint32_t)(64 - value));
    LL_RTC_EnableWriteProtection(RTC);
#endif
}

#ifdef STM32F1
void RTC_IRQHandler(void)
{
    if (LL_RTC_IsEnabledIT_SEC(RTC) != 0) {
        // Clear the RTC Second interrupt
        LL_RTC_ClearFlag_SEC(RTC);

        // Callback
        rtcTime = LL_RTC_TIME_Get(RTC) + 1;
        if (rtcCb) {
            rtcCb();
        }
    }
}
#endif

void rtcGetTime(RTC_type *rtc)
{
    getRtc(rtc);
}

void rtcSetTime(RtcMode mode, int8_t value)
{
    RTC_type rtc;
    getRtc(&rtc);

    rtcUpdate(&rtc, mode, value);
}

void rtcChangeTime(RtcMode mode, int8_t diff)
{
    RTC_type rtc;
    getRtc(&rtc);

    int8_t value = *((int8_t *)&rtc + mode);
    value += diff;

    rtcUpdate(&rtc, mode, value);
}

void rtcEditTime(RtcMode mode, int8_t digit)
{
    RTC_type rtc;
    getRtc(&rtc);

    int8_t value = *((int8_t *)&rtc + mode);
    int8_t timeMax = *((const int8_t *)&rtcMax + mode);

    if (mode == RTC_DATE) {
        timeMax = rtcDaysInMonth(&rtc);
    }

    value %= 10;
    value *= 10;
    if (value > timeMax)
        value = 0;
    value += digit;

    rtcUpdate(&rtc, mode, value);
}

void rtcSetRaw(uint32_t value)
{
    rtcTime = value;
}

uint32_t rtcGetRaw(void)
{
    return rtcTime;
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
    getRtc(&rtc);

    uint8_t alarmN = 0;

    if (rtc.hour == alarm[alarmN].hour &&
        rtc.min == alarm[alarmN].min &&
        rtc.sec == 0 &&
        rtcIsAlarmDay(alarm[alarmN].days, rtc.wday)) {
        return true;
    }

    return false;
}

void rtcChangeAlarm(AlarmMode alarmMode,  int8_t diff)
{
    Alarm *alarm = rtcGetAlarm(0);
    switch (alarmMode) {
    case ALARM_HOUR:
        alarm->hour += diff;
        if (alarm->hour > 23) {
            alarm->hour = 23;
        } else if (alarm->hour < 0) {
            alarm->hour = 0;
        }
        break;
    case ALARM_MIN:
        alarm->min += diff;
        if (alarm->min > 59) {
            alarm->min = 59;
        } else if (alarm->min < 0) {
            alarm->min = 0;
        }
        break;
    case ALARM_DAYS:
        alarm->days += diff;
        if (alarm->days >= ALARM_DAY_END) {
            alarm->days = ALARM_DAY_END - 1;
        } else if (alarm->days < ALARM_DAY_OFF) {
            alarm->days = ALARM_DAY_OFF;
        }
        break;
    }
}

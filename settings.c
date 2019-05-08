#include "settings.h"

#include "rtc.h"

static uint8_t silenceTimer = 0;

void settingsInit(void)
{
    settingsSet(EE_ALARM_HOUR, eeRead(EE_ALARM_HOUR));
    settingsSet(EE_ALARM_MINUTE, eeRead(EE_ALARM_MINUTE));
    settingsSet(EE_ALARM_DAYS, eeRead(EE_ALARM_DAYS));

    settingsSet(EE_SYSTEM_SIL_TIM, eeRead(EE_SYSTEM_SIL_TIM));
}

int16_t settingsGet(EE_Param param)
{
    int16_t ret = 0;

    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {
    case EE_ALARM_HOUR:
        ret = alarm->hour;
        break;
    case EE_ALARM_MINUTE:
        ret = alarm->min;
        break;
    case EE_ALARM_DAYS:
        ret = (int16_t)alarm->days;
        break;
    case EE_SYSTEM_SIL_TIM:
        ret = silenceTimer;
        break;
    default:
        break;
    }

    return  ret;
}

void settingsSet(EE_Param param, int16_t value)
{
    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {
    case EE_ALARM_HOUR:
        alarm->hour = (int8_t)value;
        break;
    case EE_ALARM_MINUTE:
        alarm->min = (int8_t)value;
        break;
    case EE_ALARM_DAYS:
        alarm->days = (AlarmDay)value;
        break;
    case EE_SYSTEM_SIL_TIM:
        silenceTimer = (uint8_t)value;
        break;
    default:
        break;
    }
}

void settingsStore(EE_Param param)
{
    eeUpdate(param, settingsGet(param));
}

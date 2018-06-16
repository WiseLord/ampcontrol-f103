#include "rtc.h"

#include <time.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_rtc.h>

void rtcInit()
{
    // Power interface clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    // Backup interface clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);

    // Enable access to the backup domain
    LL_PWR_EnableBkUpAccess();

    // Check if RTC has been enabled or not
    if (LL_RCC_IsEnabledRTC() == 0) {

        // Backup domain reset
        LL_RCC_ForceBackupDomainReset();
        LL_RCC_ReleaseBackupDomainReset();

        LL_RCC_LSE_Enable();

        while (LL_RCC_LSE_IsReady() != 1);

        if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE) {
            LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
        }

        LL_RCC_EnableRTC();

    }

    LL_RTC_EnableIT_SEC(RTC);
}

uint32_t rtcToSec(RTC_type *rtc)
{
    uint8_t a;
    uint8_t y;
    uint8_t m;
    uint32_t time;

    a = ((rtc->month < 3) ? 1 : 0);
    y = rtc->year + 1 - a;
    m = rtc->month + (12 * a) - 3;

    time = rtc->date - 1;
    time += (153 * m + 2) / 5;
    time += (1461 * y + 3) / 4;
    time -= 306;

    time *= 60 * 60 * 24;

    time += (rtc->hour * 3600);
    time += (rtc->min * 60);
    time += (rtc->sec);

    return time;
}

void secToRtc(uint32_t time, RTC_type *rtc)
{
    uint8_t a;
    uint8_t y;
    uint8_t m;

    rtc->sec = time % 60;
    time /= 60;
    rtc->min = time % 60;
    time /= 60;
    rtc->hour = time % 24;
    time /= 24;

    time += 306;

    rtc->wday = (time + 1) % 7;

    y = (time * 4) / 1461;
    time -= (1461 * y + 3) / 4;
    m = ((time * 5 + 2) / 153);
    time -= (153 * m + 2) / 5;

    a = ((m < 10) ? 1 : 0);

    rtc->year = y - a;
    rtc->month = m + (12 * a) - 9;
    rtc->date = time + 1;
}

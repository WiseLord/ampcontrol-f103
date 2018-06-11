#include "rtc.h"

#include <time.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_rtc.h>

#define JULIAN_DATE_BASE    2440588

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

void rtcFromUnix(uint32_t time, RTC_type *rtc)
{
    uint32_t a, b, c, d, e, m;
    uint64_t jdn = 0;

    jdn = (((time + 43200) / (86400 >> 1)) + (2440587 << 1) + 1) >> 1;

    rtc->sec = time % 60;
    time /= 60;
    rtc->min = time % 60;
    time /= 60;
    rtc->hour = time % 24;
    time /= 24;

    rtc->wday = jdn % 7;

    a = jdn + 32044;
    b = (4 * a + 3) / 146097;
    c = a - (146097 * b) / 4;
    d = (4 * c + 3) / 1461;
    e = c - (1461 * d) / 4;
    m = (5 * e + 2) / 153;

    rtc->date = e - (153 * m + 2) / 5 + 1;
    rtc->month = m + 3 - 12 * (m / 10);
    rtc->year = 100 * b + d - 4800 + (m / 10);
}

uint32_t rtcToUnix(RTC_type *rtc)
{
    uint8_t a;
    uint16_t y;
    uint8_t m;
    uint32_t jdn;

    a = (14 - rtc->month) / 12;
    y = rtc->year + 4800 - a;
    m = rtc->month + (12 * a) - 3;

    jdn = rtc->date;
    jdn += (153 * m + 2) / 5;
    jdn += 365 * y;
    jdn += y / 4;
    jdn += -y / 100;
    jdn += y / 400;
    jdn -= 32045;
    jdn -= JULIAN_DATE_BASE;
    jdn *= 86400;
    jdn += (rtc->hour * 3600);
    jdn += (rtc->min * 60);
    jdn += (rtc->sec);

    return jdn;
}

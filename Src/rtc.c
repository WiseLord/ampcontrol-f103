#include "rtc.h"

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

#include "handlers.h"

#include "stm32f1xx_ll_exti.h"
#include <stm32f1xx_ll_rtc.h>
#include <stm32f1xx_ll_tim.h>

#include "display/dispdrv.h"
#include "input.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "spectrum.h"
#include "swtimers.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1) {
    }
}

void MemManage_Handler(void)
{
    while (1) {
    }
}

void BusFault_Handler(void)
{
    while (1) {
    }
}

void UsageFault_Handler(void)
{
    while (1) {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    inputPoll();
    swTimUpdate();
}

void RTC_IRQHandler(void)
{
    if (LL_RTC_IsEnabledIT_SEC(RTC) != 0) {
        // Clear the RTC Second interrupt
        LL_RTC_ClearFlag_SEC(RTC);

        // Callback
        rtcIRQ();
    }
}

void TIM2_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM2);

        // Callbacks
        DISPLAY_IRQ();
        dispdrvPwm();
        spConvertADC();
    }
}

void TIM3_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM3)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM3);

        rcOvfIRQ();
    }
}

void EXTI9_5_IRQHandler()
{
    if (LL_EXTI_IsActiveFlag_0_31(RC_ExtiLine) != RESET) {
        // Clear RC line interrupt
        LL_EXTI_ClearFlag_0_31(RC_ExtiLine);

        // Callback
        rcIRQ();
    }
}

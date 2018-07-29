#include "handlers.h"

#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_rtc.h>
#include <stm32f1xx_ll_tim.h>

#include "display/gdfb.h"
#include "input.h"
#include "rtc.h"
#include "timers.h"

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
        rtcReadTime();

        // Wait until last write operation on RTC registers has finished
        LL_RTC_WaitForSynchro(RTC);
    }

    // Clear the EXTI's Flag for RTC Alarm
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
}

void TIM2_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM2);

        // Callback
        ks0108IRQ();
    }
}

void ADC1_2_IRQHandler(void)
{

}

void DMA1_Channel1_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC1(DMA1) == 1)
  {
    // Clear flag DMA global interrupt
    // (global interrupt flag: half transfer and transfer complete flags)
    LL_DMA_ClearFlag_GI1(DMA1);

    // DMA callback
    //
  }
}

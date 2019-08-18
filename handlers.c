#include "handlers.h"

#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_rtc.h>
#include <stm32f1xx_ll_tim.h>
#include <stm32f1xx_ll_usart.h>

#include "control.h"
#include "display/dispdrv.h"
#include "input.h"
#include "karadio.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "spectrum.h"
#include "swtimers.h"
#include "usb/usbd_conf.h"

static int32_t sysTimer = 0;

int32_t getSysTimer(void)
{
    return sysTimer;
}

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
    dispdrvBusIRQ();
    inputPoll();
    swTimUpdate();

    sysTimer++;
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
        dispdrvPwm();
        spConvertADC();
    }
}

void USART1_IRQHandler(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1)) {
        controlIRQ();
    } else {
        // Call Error function
    }
}

void USART2_IRQHandler(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2)) {
        karadioIRQ();
    } else {
        // Call Error function
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

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USBD_IRQ();
}

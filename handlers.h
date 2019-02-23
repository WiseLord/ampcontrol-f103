#ifndef HANDLERS_H
#define HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);

void SysTick_Handler(void);
void RTC_IRQHandler(void);
void TIM2_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void USB_LP_CAN1_RX0_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // HANDLERS_H

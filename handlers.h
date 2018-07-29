#ifndef STM32F1XX_IT_H
#define STM32F1XX_IT_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_exti.h"

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

void ADC1_2_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);

#endif // STM32F1XX_IT_H

#ifndef HWLIBS_H
#define HWLIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F103xB)

#include <stm32f1xx_hal_def.h>
#include <stm32f1xx_hal_pcd.h>
#include <stm32f1xx_ll_adc.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_i2c.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_rtc.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_tim.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_utils.h>

#elif defined (STM32F303xC)

#include <stm32f3xx_hal_def.h>
#include <stm32f3xx_hal_pcd.h>
#include <stm32f3xx_ll_adc.h>
#include <stm32f3xx_ll_bus.h>
#include <stm32f3xx_ll_cortex.h>
#include <stm32f3xx_ll_dma.h>
#include <stm32f3xx_ll_exti.h>
#include <stm32f3xx_ll_gpio.h>
#include <stm32f3xx_ll_i2c.h>
#include <stm32f3xx_ll_pwr.h>
#include <stm32f3xx_ll_rcc.h>
#include <stm32f3xx_ll_rtc.h>
#include <stm32f3xx_ll_spi.h>
#include <stm32f3xx_ll_system.h>
#include <stm32f3xx_ll_tim.h>
#include <stm32f3xx_ll_usart.h>
#include <stm32f3xx_ll_utils.h>

#endif

// Project-specific definitions
#define EXTI_RC_HANDLER         EXTI9_5_IRQHandler

#define I2C_AMP                 I2C1

#define LL_I2C1_SCL_PIN         LL_GPIO_PIN_8
#define LL_I2C1_SDA_PIN         LL_GPIO_PIN_9

#define USART_DBG               USART1
#define USART_DBG_HANDLER       USART1_IRQHandler

#define USART_MPC               USART2
#define USART_MPC_HANDLER       USART2_IRQHandler

#define TIM_SPECTRUM            TIM2
#define TIM_SPECTRUM_HANDLER    TIM2_IRQHandler

#define TIM_RC                  TIM3
#define TIM_RC_HANDLER          TIM3_IRQHandler

#define TIM_INPUT               TIM4
#define TIM_INPUT_HANDLER       TIM4_IRQHandler

#ifdef __cplusplus
}
#endif

#endif // HWLIBS_H

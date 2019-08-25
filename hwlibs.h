#ifndef HWLIBS_H
#define HWLIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _STM32F1
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
#endif

#ifdef _STM32F3
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

#ifdef __cplusplus
}
#endif

#endif // HWLIBS_H

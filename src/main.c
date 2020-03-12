#include "amp.h"
#include "hwlibs.h"

static void NVIC_Init(void)
{
    // System interrupt init
    NVIC_SetPriority(MemoryManagement_IRQn, 0);
    NVIC_SetPriority(BusFault_IRQn, 0);
    NVIC_SetPriority(UsageFault_IRQn, 0);
    NVIC_SetPriority(SVCall_IRQn, 0);
    NVIC_SetPriority(DebugMonitor_IRQn, 0);
    NVIC_SetPriority(PendSV_IRQn, 0);
}

void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    // Configure HSE
    LL_RCC_HSE_Enable();
    // Wait till HSE is ready
    while (LL_RCC_HSE_IsReady() != 1);

    // Configure PLL
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();
    // Wait till PLL is ready
    while (LL_RCC_PLL_IsReady() != 1);

    // Configure system clock
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    // Wait till System clock is ready
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    // Configure APB1/APB2 clock
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    // Configure ADC clock
#ifdef STM32F1
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_8);
#endif
#ifdef STM32F3
    LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSRC_PLL_DIV_8);
#endif

    // Configure SysTick
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);

    // SysTick_IRQn interrupt configuration
    NVIC_SetPriority(SysTick_IRQn, 0);
}

static void sysInit(void)
{
    // System
    NVIC_Init();
    SystemClock_Config();

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    // Enable clock for all GPIO peripherials
#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
#endif
#ifdef STM32F3
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#endif

#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif

#ifdef STM32F3
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_SYSCLK);
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_HSI);
#endif
}

int main(void)
{
    sysInit();

    ampInit();
    ampRun();

    return 0;
}

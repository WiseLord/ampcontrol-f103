#include "hwlibs.h"

#include "actions.h"
#include "control.h"
#include "debug.h"
#include "handlers.h"
#include "input.h"
#include "karadio.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "screen.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "timers.h"
#include "usb/usbhid.h"
#include "utils.h"

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0    ((uint32_t)0x00000007)
#define NVIC_PRIORITYGROUP_1    ((uint32_t)0x00000006)
#define NVIC_PRIORITYGROUP_2    ((uint32_t)0x00000005)
#define NVIC_PRIORITYGROUP_3    ((uint32_t)0x00000004)
#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)
#endif

static void LL_Init(void)
{
#ifdef _STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    // System interrupt init
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

static void SystemClock_Config(void)
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

    // Configure SysTick
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);
    // SysTick_IRQn interrupt configuration
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

int main(void)
{
    // System
    LL_Init();
    SystemClock_Config();

    settingsInit();
    pinsInit();

    dbgInit();

    usbHidInit();
    screenInit();
    spInit();

    inputInit();
    rcInit();
    rtcInit();

    controlInit();
    karadioInit();

    swTimInit();
    LL_SYSTICK_EnableIT();
    timersInit();

#ifdef _DEBUG_FPS
    int32_t fpsCnt = 0;
#endif

    while (1) {
        actionHandle(ACTION_VISIBLE);
        controlGetData();
        karadioGetData();
        screenShow(false);
        actionUserGet();
#ifdef _DEBUG_FPS
        int32_t cnt = getSysTimer();
        fpsCnt = (cnt == fpsCnt) ? 1000 : 1000 / (cnt - fpsCnt);
        dbg(utilMkStr("FPS: %d", fpsCnt));
        fpsCnt = cnt;
#endif
    }
}

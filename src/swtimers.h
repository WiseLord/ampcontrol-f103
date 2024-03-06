#ifndef SWTIMERS_H
#define SWTIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SW_TIM_OFF      -1
#define SW_TIM_ON       0

typedef uint8_t SwTimer;
enum {
    // Decreasing timers
    SW_TIM_DISPLAY = 0,
    SW_TIM_SP_CONVERT,
    SW_TIM_AMP_INIT,
    SW_TIM_RTC_INIT,
    SW_TIM_STBY,
    SW_TIM_SILENCE,
    SW_TIM_CHECK_SIGNAL,
    SW_TIM_SOFT_VOLUME,
    SW_TIM_INPUT_POLL,
    SW_TIM_RC_REPEAT,
    SW_TIM_RC_NOACION,
    SW_TIM_RDS_HOLD,
    SW_TIM_GPIO_KEY,
    SW_TIM_MPD_POWEROFF,
    SW_TIM_DIGIT_INPUT,
    SW_TIM_SCROLL,
    SW_TIM_VOLUME_KEY,

    SW_TIM_DEC_END,

    // Increasing timers
    SW_TIM_SYSTEM = SW_TIM_DEC_END,

    SW_TIM_END,
};

void swTimInit(void);

void swTimSet(SwTimer timer, int32_t value);
int32_t swTimGet(SwTimer timer);

#ifdef __cplusplus
}
#endif

#endif // SWTIMERS_H

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
    SW_TIM_DISPLAY = 0,
    SW_TIM_SP_CONVERT,
    SW_TIM_INPUT_POLL,
    SW_TIM_AMP_INIT,
    SW_TIM_RC_REPEAT,
    SW_TIM_RC_NOACION,
    SW_TIM_STBY_TIMER,
    SW_TIM_SILENCE_TIMER,
    SW_TIM_RTC_INIT,
    SW_TIM_SOFT_VOLUME,
    SW_TIM_BT_KEY,

    SW_TIM_DEC_END,

    SW_TIM_SYSTEM,

    SW_TIM_END,
};

void swTimInit(void);

void swTimSet(SwTimer timer, int32_t value);
int32_t swTimGet(SwTimer timer);

#ifdef __cplusplus
}
#endif

#endif // SWTIMERS_H

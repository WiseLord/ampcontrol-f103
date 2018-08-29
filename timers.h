#ifndef TIMERS_H
#define TIMERS_H

#include <inttypes.h>

#define SW_TIM_OFF       -1

void timersInit(void);

void swTimUpdate(void);

void swTimSetDisplay(int16_t value);
int16_t swTimGetDisplay(void);

#endif // TIMERS_H

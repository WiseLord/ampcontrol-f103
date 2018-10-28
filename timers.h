#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

#define SW_TIM_OFF       -1

void timersInit(void);

void swTimUpdate(void);

void swTimSetDisplay(int16_t value);
int16_t swTimGetDisplay(void);

void swTimSetSpConvert(int16_t value);
int16_t swTimGetSpConvert(void);

void swTimSetTunerPoll(int16_t value);
int16_t swTimGetTunerPoll(void);

void swTimSetInitHw(int16_t value);
int16_t swTimGetInitHw(void);

#endif // TIMERS_H

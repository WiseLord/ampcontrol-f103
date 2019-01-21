#ifndef SWTIMERS_H
#define SWTIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SW_TIM_OFF       -1

void swTimUpdate(void);

void swTimSetDisplay(int16_t value);
int16_t swTimGetDisplay(void);

void swTimSetSpConvert(int16_t value);
int16_t swTimGetSpConvert(void);

void swTimSetTunerPoll(int16_t value);
int16_t swTimGetTunerPoll(void);

void swTimSetInitHw(int16_t value);
int16_t swTimGetInitHw(void);

void swTimSetRcRepeat(int16_t value);
int16_t swTimGetRcRepeat(void);

#ifdef __cplusplus
}
#endif

#endif // SWTIMERS_H
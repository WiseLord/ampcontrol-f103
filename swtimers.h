#ifndef SWTIMERS_H
#define SWTIMERS_H

#include <inttypes.h>

#define SW_TIM_OFF       -1

void swTimUpdate(void);

void swTimSetDisplay(int16_t value);
int16_t swTimGetDisplay();

#endif // SWTIMERS_H

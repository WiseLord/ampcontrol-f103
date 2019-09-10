#ifndef TIMERS_H
#define TIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void timerInit(void *tim, uint32_t prescaler, uint32_t reload);
void timersInit(void);

#ifdef __cplusplus
}
#endif

#endif // TIMERS_H

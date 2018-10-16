#ifndef RDA5807X_H
#define RDA5807X_H

#include <inttypes.h>

#define RDA580X_VOL_MIN             0
#define RDA580X_VOL_MAX             15

void rda580xInit(void);

void rda580xSetFreq(uint16_t value);
void rda580xSeek(int8_t direction);

void rda580xSetMute(uint8_t value);
void rda580xSetVolume(int8_t value);

void rda580xSetPower(uint8_t value);

void rda580xUpdateStatus(void);

uint16_t rda580xGetFreq(void);

#endif // RDA5807X_H

#ifndef RDA5807X_H
#define RDA5807X_H

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

#define RDA580X_VOL_MIN             0
#define RDA580X_VOL_MAX             15

void rda580xInit(TunerParam *param);

void rda580xSetFreq(uint16_t value);
void rda580xSeek(int8_t direction);

void rda580xSetVolume(int8_t value);
void rda580xSetMute(bool value);
void rda580xSetBassBoost(bool value);
void rda580xSetForcedMono(bool value);
void rda580xSetRds(bool value);

void rda580xSetPower(bool value);

void rda580xUpdateStatus(void);

uint16_t rda580xGetFreq(void);

#endif // RDA5807X_H

#ifndef RDA5807X_H
#define RDA5807X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

#define RDA580X_VOL_MIN             0
#define RDA580X_VOL_MAX             15

const TunerApi *rda580xGetApi(void);

void rda580xInit(TunerParam *param, TunerStatus *status);

void rda580xSetFreq(uint16_t value);
void rda580xSeek(int8_t direction);

void rda580xSetVolume(int8_t value);
void rda580xSetMute(bool value);
void rda580xSetBassBoost(bool value);
void rda580xSetForcedMono(bool value);
void rda580xSetRds(bool value);

void rda580xSetPower(bool value);

void rda580xUpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // RDA5807X_H

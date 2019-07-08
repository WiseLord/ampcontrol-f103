#ifndef SI470X_H
#define SI470X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

#define SI470X_VOL_MIN              0
#define SI470X_VOL_MAX              15

const TunerApi *si470xGetApi(void);

void si470xInit(TunerParam *param, TunerStatus *status);

void si470xSetFreq(uint16_t value);
void si470xSeek(int8_t direction);

void si470xSetVolume(int8_t value);
void si470xSetMute(bool value);
void si470xSetForcedMono(bool value);
void si470xSetRds(bool value);

void si470xSetPower(bool value);

void si470xUpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // SI470X_H

#ifndef SI470X_H
#define SI470X_H

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

#define SI470X_VOL_MIN              0
#define SI470X_VOL_MAX              15

void si470xInit(TunerParam *param);

void si470xSetFreq(uint16_t value);
void si470xSeek(int8_t direction);

void si470xSetVolume(int8_t value);
void si470xSetMute(bool value);
void si470xSetForcedMono(bool value);
void si470xSetRds(bool value);

void si470xSetPower(bool value);

void si470xUpdateStatus(void);

uint16_t si470xGetFreq(void);

#endif // SI470X_H

#ifndef TUX03X_H
#define TUX03X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

const TunerApi *tux03xGetApi(void);

void tux03xInit(TunerParam *param, TunerStatus *status);

void tux03xSetFreq(uint16_t value);

void tux03xSetForcedMono(bool value);

void tux03xSetPower(bool value);

void tux03xUpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // TUX03X_H

#ifndef LC7213X_H
#define LC7213X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

const TunerApi *lc7213xGetApi(void);

void lc7213xInit(TunerParam *param, TunerStatus *status);

void lc7213xSetFreq(uint16_t freq);

void lc7213xSetMute(bool value);
void lc7213xSetForcedMono(bool value);

void lc7213xUpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // LC7213X_H

#ifndef TDA731X_H
#define TDA731X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

const AudioApi *tda731xGetApi(void);

void tda731xInit(AudioParam *param);
int8_t tda731xGetInCnt(void);

void tda731xSetTune(AudioTune tune, int8_t value);
void tda731xSetInput(int8_t value);
void tda731xSetMute(bool value);
void tda731xSetLoudness(bool value);

#ifdef __cplusplus
}
#endif

#endif // TDA731X_H

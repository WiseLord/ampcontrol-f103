#ifndef PT232X_H
#define PT232X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define PT2323_IN_CNT       5

const AudioApi *pt232xGetApi(void);

void pt232xInitParam(AudioParam *param);
void pt232xReset(void);

void pt232xSetTune(AudioTune tune, int8_t value);
void pt232xSetInput(int8_t value);
void pt232xSetMute(bool value);

void pt232xSetEffect3D(bool value);
void pt232xSetBypass(bool value);
void pt232xSetSurround(bool value);

#ifdef __cplusplus
}
#endif

#endif // PT232X_H

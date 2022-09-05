#ifndef RELAY_H
#define RELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "audiodefs.h"

const AudioApi *relayGetApi(void);

void relayInit(AudioParam *param);
int8_t relayGetInCnt(void);

void relaySetTune(AudioTune tune, int8_t value);
void relaySetInput(int8_t value);

void relaySetLoudness(bool value);

#ifdef __cplusplus
}
#endif

#endif // RELAY_H

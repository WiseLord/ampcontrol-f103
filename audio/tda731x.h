#ifndef TDA731X_H
#define TDA731X_H

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

void tda731xInit(AudioParam *param);

void tda731xSetTune(AudioTune tune, int8_t value);
void tda731xSetInput(uint8_t value);
void tda731xSetMute(bool value);
void tda731xSetLoudness(bool value);

#endif // TDA731X_H

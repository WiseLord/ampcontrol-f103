#ifndef PT232X_H
#define PT232X_H

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

void pt232xInit(AudioParam *param);

void pt232xSetTune(AudioTune tune, int8_t value);
void pt232xSetInput(uint8_t value);
void pt232xSetMute(bool value);

#endif // PT232X_H
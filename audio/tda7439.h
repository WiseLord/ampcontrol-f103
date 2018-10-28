#ifndef TDA7439_H
#define TDA7439_H

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

void tda7439Init(AudioParam *param);

void tda7439SetTune(AudioTune tune, int8_t value);
void tda7439SetFlag(AudioFlag flag, bool value);
void tda7439SetInput(uint8_t value);
void tda7439SetMute(bool value);

#endif // TDA7439_H

#ifndef TDA7440_H
#define TDA7440_H

#ifdef __cplusplus
}
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"
#include "tda7439.h"

#define TDA7440_IN_CNT      TDA7439_IN_CNT

void tda7440Init(AudioParam *param);

void tda7440SetTune(AudioTune tune, int8_t value);
void tda7440SetInput(uint8_t value);
void tda7440SetMute(bool value);

#ifdef __cplusplus
extern "C" {
#endif

#endif // TDA7440_H

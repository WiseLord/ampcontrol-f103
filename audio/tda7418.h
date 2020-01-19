#ifndef TDA7418_H
#define TDA7418_H

#ifdef __cplusplus
}
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define TDA7418_IN_CNT      4

const AudioApi *tda7418GetApi(void);

void tda7418Init(AudioParam *param);

void tda7418SetTune(AudioTune tune, int8_t value);
void tda7418SetInput(int8_t value);
void tda7418SetMute(bool value);
void tda7418SetLoudness(bool value);

#ifdef __cplusplus
extern "C" {
#endif

#endif // TDA7418_H

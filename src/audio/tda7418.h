#ifndef TDA7418_H
#define TDA7418_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

const AudioApi *tda7418GetApi(void);

void tda7418Init(AudioParam *param);
int8_t tda7418GetInCnt(void);

void tda7418SetTune(AudioTune tune, int8_t value);
void tda7418SetInput(int8_t value);
void tda7418SetMute(bool value);

#ifdef __cplusplus
}
#endif

#endif // TDA7418_H

#ifndef TDA7719_H
#define TDA7719_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

const AudioApi *tda7719GetApi(void);

void tda7719Init(AudioParam *param);
int8_t tda7719GetInCnt(void);

void tda7719SetTune(AudioTune tune, int8_t value);
void tda7719SetInput(int8_t value);
void tda7719SetMute(bool value);

#ifdef __cplusplus
}
#endif

#endif // TDA7719_H

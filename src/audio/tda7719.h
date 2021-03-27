#ifndef TDA7719_H
#define TDA7719_H

#ifdef __cplusplus
}
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define TDA7719_IN_CNT      6

const AudioApi *tda7719GetApi(void);

void tda7719Init(AudioParam *param);

void tda7719SetTune(AudioTune tune, int8_t value);
void tda7719SetInput(int8_t value);
void tda7719SetMute(bool value);

#ifdef __cplusplus
extern "C" {
#endif

#endif // TDA7719_H

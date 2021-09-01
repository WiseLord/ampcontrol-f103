#ifndef TDA7439_H
#define TDA7439_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

const AudioApi *tda7439GetApi(void);

void tda7439Init(AudioParam *param);
int8_t tda7439GetInCnt(void);

void tda7439SetTune(AudioTune tune, int8_t value);
void tda7439SetInput(int8_t value);
void tda7439SetMute(bool value);

#ifdef __cplusplus
}
#endif

#endif // TDA7439_H

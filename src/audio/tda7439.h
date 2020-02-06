#ifndef TDA7439_H
#define TDA7439_H

#ifdef __cplusplus
}
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define TDA7439_IN_CNT      4

const AudioApi *tda7439GetApi(void);

void tda7439InitParam(AudioParam *param);

void tda7439SetTune(AudioTune tune, int8_t value);
void tda7439SetInput(int8_t value);
void tda7439SetMute(bool value);

#ifdef __cplusplus
extern "C" {
#endif

#endif // TDA7439_H

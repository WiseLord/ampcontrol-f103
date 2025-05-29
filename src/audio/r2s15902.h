#ifndef R2S15902_H
#define R2S15902_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

const AudioApi *r2s15902GetApi(void);

void r2s15902Init(AudioParam *param);
int8_t r2s15902GetInCnt(void);

void r2s15902SetTune(AudioTune tune, int8_t value);
void r2s15902SetInput(int8_t value);

void r2s15902SetMute(bool value);

#ifdef __cplusplus
}
#endif

#endif // R2S15902_H

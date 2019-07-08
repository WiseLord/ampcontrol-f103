#ifndef TEA5767_H
#define TEA5767_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

const TunerApi *tea5767GetApi(void);

void tea5767Init(TunerParam *param, TunerStatus *status);

void tea5767SetFreq(uint16_t value);
void tea5767Seek(int8_t direction);

void tea5767SetMute(bool value);

void tea5767SetPower(bool value);

void tea5767UpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // TEA5767_H

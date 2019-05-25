#ifndef QN8006_H
#define QN8006_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

void qn8006Init(TunerParam *param, TunerStatus *status);

void qn8006SetFreq(uint16_t value);
void qn8006Seek(int8_t direction);

void qn8006SetMute(bool value);

void qn8006SetPower(bool value);

void qn8006UpdateStatus(void);

#ifdef __cplusplus
}
#endif

#endif // QN8006_H

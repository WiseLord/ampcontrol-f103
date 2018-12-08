#ifndef TEA5767_H
#define TEA5767_H

#include <stdint.h>
#include <stdbool.h>

#include "tunerdefs.h"

void tea5767Init(TunerParam *param);

void tea5767SetFreq(uint16_t value);
void tea5767Seek(int8_t direction);

void tea5767SetMute(bool value);

void tea5767SetPower(bool value);

void tea5767UpdateStatus(void);

uint16_t tea5767GetFreq(void);

#endif // TEA5767_H

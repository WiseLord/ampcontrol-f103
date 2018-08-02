#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <inttypes.h>

void spInit(void);

void spGetADC(uint8_t *dataL, uint8_t *dataR, uint8_t speed);

void spConvertADC();

#endif // SPECTRUM_H

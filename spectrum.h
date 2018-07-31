#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <inttypes.h>

void spInit(void);

void spGetADC(uint8_t **dataL, uint8_t **dataR);

void spConvertADC();

void spUpdate();

#endif // SPECTRUM_H

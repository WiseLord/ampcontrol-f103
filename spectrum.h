#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <inttypes.h>

void spInit(void);

void spGetADC(uint16_t **dataL, uint16_t **dataR);

void spConvertADC();

void spUpdate();

#endif // SPECTRUM_H

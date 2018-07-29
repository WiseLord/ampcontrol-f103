#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <inttypes.h>

void spInit(void);

uint16_t *spGetADC(void);
void spConvertADC();

#endif // SPECTRUM_H

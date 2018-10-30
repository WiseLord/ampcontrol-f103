#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <stdint.h>

#include "fft.h"

enum {
    SP_CHAN_LEFT = 0,
    SP_CHAN_RIGHT,

    SP_CHAN_END
};

typedef struct {
    uint8_t data[FFT_SIZE / 2];
    uint8_t fall[FFT_SIZE / 2];
    uint8_t show[FFT_SIZE / 2];
    uint8_t peak[FFT_SIZE / 2];
    uint8_t old_show[FFT_SIZE / 2];
    uint8_t old_peak[FFT_SIZE / 2];
} SpectrumData;

void spInit(void);

void spGetADC(uint8_t *dataL, uint8_t *dataR);

void spConvertADC();

#endif // SPECTRUM_H

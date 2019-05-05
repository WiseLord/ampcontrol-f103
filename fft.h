#ifndef FFT_H
#define FFT_H

#include <stdint.h>

#define FFT_SIZE    1024
#define FFT_LOG2    10

typedef struct {
    int16_t fr;
    int16_t fi;
} FftSample;

int16_t fft_sin(int16_t phi);
int16_t fft_cos(int16_t phi);

void fft_rev_bin(FftSample *sp);
void fft_radix4(FftSample *sp);

#endif // FFT_H

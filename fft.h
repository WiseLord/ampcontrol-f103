#ifndef FFT_H
#define FFT_H

#include <stdint.h>

#define FFT_SIZE    1024
#define FFT_LOG2    10

#define N_WAVE      1024
#define N_HANN      1024
#define N_DB        256

typedef struct
{
    int16_t fr;
    int16_t fi;
} FftSample;

int16_t fft_sin(int16_t phi);
int16_t fft_cos(int16_t phi);

void fft_hamm_window(FftSample *sp);
void fft_rev_bin(FftSample *sp);
void fft_radix4(FftSample *sp);
void fft_cplx2dB(FftSample *sp, uint8_t *out);

#endif // FFT_H

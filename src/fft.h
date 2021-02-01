#ifndef FFT_H
#define FFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FFT_SIZE    1024
#define FFT_LOG2    10

#define N_WAVE      1024

typedef struct {
    int16_t fr;
    int16_t fi;
} FftSample;

int16_t fft_sin(int16_t phi);
int16_t fft_cos(int16_t phi);

void fft_rev_bin(FftSample *sp);
void fft_hamm_window(FftSample *sp);
void fft_radix4(FftSample *sp);

#ifdef __cplusplus
}
#endif

#endif // FFT_H

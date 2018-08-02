#ifndef FFT_H
#define FFT_H

#include <inttypes.h>

#define FFT_SIZE    256
#define FFT_LOG2    8

#define N_WAVE      256
#define N_DB        32

void fft_hamm_window(int16_t *fr);
void fft_rev_bin(int16_t *fr);
void fft_radix4(int16_t *fr, int16_t *fi);
void fft_cplx2dB(int16_t *fr, int16_t *fi);

#endif // FFT_H

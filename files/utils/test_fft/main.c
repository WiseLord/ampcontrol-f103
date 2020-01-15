#include <stdio.h>
#include <math.h>

#include "../../../fft.h"

#define SIGNAL_SCALE    1023
#define SAMPLE_FREQ     20000
#define SIGNAL_FREQ     100

#define N_HANN          1024

static FftSample sp[FFT_SIZE];

void generate_data(void)
{
    for (int i = 0; i < FFT_SIZE; i++) {
        double t = (double)i / SAMPLE_FREQ;
        sp[i].fr = (int16_t)(SIGNAL_SCALE * sin(2 * M_PI * SIGNAL_FREQ * t));
    }
}

void print_data(void)
{
    printf("Data:\n");

    for (int i = 0; i < FFT_SIZE; i++) {
        printf("%6d, ", sp[i].fr);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n\n");
}

void print_result(void)
{
    printf("Data:\n");

    for (int i = 0; i < FFT_SIZE; i++) {
        uint16_t calc = (uint16_t)((sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi) >> 15);

        printf("%6d, ", calc);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n\n");
}


int main()
{
    generate_data();

    print_data();

    fft_hamm_window(sp);
    fft_rev_bin(sp);
    fft_radix4(sp);

    print_result();

    return 0;
}

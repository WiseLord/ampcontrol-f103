#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define SAMPLE_FREQ     1024000
#define SIGNAL_FREQ     1000

#define FFT_SIZE        1024

#define SIGNAL_SCALE    32767
#define HAMM_SCALE      65535

#define N_DB        128
#define DB_MAX      65535

int16_t input[FFT_SIZE];

int main()
{
    double t = 0;

    // Sinus
    for (int i = 0; i < FFT_SIZE; i++) {
        t = (double)i / SAMPLE_FREQ;

        printf("%6d, ", (int16_t)(SIGNAL_SCALE * sin(2 * M_PI * SIGNAL_FREQ * t)));
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n");

    // Hamming window
    double a0 = 0.53836;
    double a1 = 0.46164;
//    double a0 = 0.5; // Hann window
//    double a1 = 0.5; // Hann window

    for (int i = 0; i < FFT_SIZE; i++) {
        double w;
        uint16_t hw;

        w = a0 - a1 * cos((2 * M_PI * i) / (FFT_SIZE - 1));
        hw = (uint16_t)(HAMM_SCALE * w);

        printf("%6d, ", hw);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n");

    for (int i = 0; i < N_DB; i++) {
        double l;

        l = exp((double)i / (N_DB - 1) * log(DB_MAX));
        printf("%5.0f, ", l);
        if (i % 8 == 7)
            printf("\n");
    }

    return 0;
}

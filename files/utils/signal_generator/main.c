#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define SAMPLE_FREQ     1024000
#define SIGNAL_FREQ     1000

#define FFT_SIZE        1024

#define SIGNAL_SCALE    32767
#define HAMM_SCALE      65535

#define N_DB        256
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

//        double power = 1;
//        double N = 0;

//        double power = 0.9;
//        double N = 2;

//        double power = 0.8;
//        double N = 8;

//        double power = 0.7;
//        double N = 27;

//        double power = 0.6;
//        double N = 84;

//        double power = 0.5;
//        double N = 256;

        double power = 0.5;
        double N = 0;

        double x = (65535 + N) / pow (65535, power);
        l = pow(l, power) * x - N;

        printf("%5.0f, ", l);
        if (i % 8 == 7)
            printf("\n");
    }

    return 0;
}

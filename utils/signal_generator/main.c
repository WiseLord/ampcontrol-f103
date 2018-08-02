#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define SAMPLE_FREQ     256000
#define SIGNAL_FREQ     1000

#define FFT_SIZE        256

//#define SIGNAL_SCALE    2047
#define SIGNAL_SCALE    4095

int16_t input[FFT_SIZE];

int main()
{
    double t = 0;

    // Sinus
    for (int i = 0; i < FFT_SIZE; i++) {
        t = (double)i / SAMPLE_FREQ;

        input[i] = (int16_t)(SIGNAL_SCALE * sin(2 * M_PI * SIGNAL_FREQ * t));
    }

    for (int i = 0; i < FFT_SIZE; i++) {
        printf("%6d, ", input[i]);
        if (i % 8 == 7)
            printf("\n");
    }

    printf("\n");

    // Hamming window
    double a0 = 0.53836;
    double a1 = 0.46164;
//    double a0 = 0.5;
//    double a1 = 0.5;

    for (int i = 0; i < FFT_SIZE; i++) {
        double w;
        uint16_t hw;

        w = a0 - a1 * cos((2 * M_PI * i) / (FFT_SIZE - 1));
        hw = (uint16_t)(SIGNAL_SCALE * w);

        printf("%6d, ", hw);
        if (i % 8 == 7)
            printf("\n");
    }

    return 0;
}

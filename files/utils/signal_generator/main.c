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

void print_sinus_table(void)
{
    printf("Sinus:\n");

    double t = 0;

    for (int i = 0; i < FFT_SIZE / 4 + 1; i++) {
        t = (double)i / SAMPLE_FREQ;

        printf("%6d, ", (int16_t)(SIGNAL_SCALE * sin(2 * M_PI * SIGNAL_FREQ * t)));
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n\n");
}

void print_hamming_table(double a0, double a1)
{
    printf("Hamming window:\n");

    for (int i = 0; i < FFT_SIZE / 2; i++) {
        double w;
        uint16_t hw;

        w = a0 - a1 * cos((2 * M_PI * i) / (FFT_SIZE - 1));
        hw = (uint16_t)(HAMM_SCALE * w);

        printf("%6d, ", hw);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n");
}

void print_db_table(double power, int N)
{
    printf("Db table:\n");
    for (int i = 0; i < N_DB; i++) {
        double l;

        l = exp((double)i / (N_DB - 1) * log(DB_MAX));

        double x = (65535 + N) / pow (65535, power);
        l = pow(l, power) * x - N;

        printf("%5.0f, ", l);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n");
}

int main()
{
    print_sinus_table();

//    print_hamming_table(0.5, 0.5);    // Hann
    print_hamming_table(0.53836, 0.46164);  // Hamming

//    print_db_table(1.0, 0);
//    print_db_table(0.9, 2);
//    print_db_table(0.8, 8);
//    print_db_table(0.7, 27);
//    print_db_table(0.6, 84);
//    print_db_table(0.5, 256);
    print_db_table(0.5, 0);

    return 0;
}

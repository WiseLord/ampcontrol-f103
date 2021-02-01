#include "fft.h"

#define N_HANN      1024

static const int16_t sinTable[N_WAVE / 4 + 1] = {
    0,      201,    402,    603,    804,    1005,   1206,   1406,
    1607,   1808,   2009,   2209,   2410,   2610,   2811,   3011,
    3211,   3411,   3611,   3811,   4011,   4210,   4409,   4608,
    4807,   5006,   5205,   5403,   5601,   5799,   5997,   6195,
    6392,   6589,   6786,   6982,   7179,   7375,   7571,   7766,
    7961,   8156,   8351,   8545,   8739,   8932,   9126,   9319,
    9511,   9703,   9895,   10087,  10278,  10469,  10659,  10849,
    11038,  11227,  11416,  11604,  11792,  11980,  12166,  12353,
    12539,  12724,  12909,  13094,  13278,  13462,  13645,  13827,
    14009,  14191,  14372,  14552,  14732,  14911,  15090,  15268,
    15446,  15623,  15799,  15975,  16150,  16325,  16499,  16672,
    16845,  17017,  17189,  17360,  17530,  17699,  17868,  18036,
    18204,  18371,  18537,  18702,  18867,  19031,  19194,  19357,
    19519,  19680,  19840,  20000,  20159,  20317,  20474,  20631,
    20787,  20942,  21096,  21249,  21402,  21554,  21705,  21855,
    22004,  22153,  22301,  22448,  22594,  22739,  22883,  23027,
    23169,  23311,  23452,  23592,  23731,  23869,  24006,  24143,
    24278,  24413,  24546,  24679,  24811,  24942,  25072,  25201,
    25329,  25456,  25582,  25707,  25831,  25954,  26077,  26198,
    26318,  26437,  26556,  26673,  26789,  26905,  27019,  27132,
    27244,  27355,  27466,  27575,  27683,  27790,  27896,  28001,
    28105,  28208,  28309,  28410,  28510,  28608,  28706,  28802,
    28897,  28992,  29085,  29177,  29268,  29358,  29446,  29534,
    29621,  29706,  29790,  29873,  29955,  30036,  30116,  30195,
    30272,  30349,  30424,  30498,  30571,  30643,  30713,  30783,
    30851,  30918,  30984,  31049,  31113,  31175,  31236,  31297,
    31356,  31413,  31470,  31525,  31580,  31633,  31684,  31735,
    31785,  31833,  31880,  31926,  31970,  32014,  32056,  32097,
    32137,  32176,  32213,  32249,  32284,  32318,  32350,  32382,
    32412,  32441,  32468,  32495,  32520,  32544,  32567,  32588,
    32609,  32628,  32646,  32662,  32678,  32692,  32705,  32717,
    32727,  32736,  32744,  32751,  32757,  32761,  32764,  32766,
    32767
};

__attribute__((always_inline))
static inline void sum_dif(int16_t a, int16_t b,
                           int16_t *s, int16_t *d)
{
    *s = a + b;
    *d = a - b;
}

__attribute__((always_inline))
static inline void mult_shf(int16_t cos, int16_t sin, int16_t x, int16_t y,
                            int16_t *u, int16_t *v)
{
    *u = (x * cos - y * sin) >> 15;
    *v = (y * cos + x * sin) >> 15;
}

int16_t fft_sin(int16_t phi)
{
    return ((phi & (N_WAVE / 2)) ? -1 : 1) *
           sinTable[(phi & (N_WAVE / 4)) ? ((N_WAVE / 2) - (phi & (N_WAVE / 2 - 1))) :
                                         (phi & (N_WAVE / 4 - 1))];
}

int16_t fft_cos(int16_t phi)
{
    return fft_sin(phi + N_WAVE / 4);
}

void fft_rev_bin(FftSample *sp)
{
    int16_t m, mr = 0;
    int16_t tr, l;

    for (m = 1; m < FFT_SIZE; m++) {
        l = FFT_SIZE;
        do {
            l >>= 1;
        } while (mr + l >= FFT_SIZE);

        mr = (mr & (l - 1)) + l;

        if (mr <= m) {
            continue;
        }
        tr = sp[m].fr;
        sp[m].fr = sp[mr].fr;
        sp[mr].fr = tr;
    }
}

void fft_hamm_window(FftSample *sp)
{
    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {

        // Use Hamm coefficients 0.53836 and 0.46164 scaled by SIN_SCALE = 2^15
        uint16_t ht = (uint16_t)(35281 - ((30253 * fft_cos(i * (N_HANN / FFT_SIZE))) >> 15));

        sp[i].fr = (ht * sp[i].fr) >> 16;
        sp[FFT_SIZE - 1 - i].fr = (ht * sp[FFT_SIZE - 1 - i].fr) >> 16;
    }
}

void fft_radix4(FftSample *sp)
{
    int16_t ldm = 0, rdx = 2;

    for (int16_t i0 = 0; i0 < FFT_SIZE; i0 += 4) {
        int16_t i1 = i0 + 1;
        int16_t i2 = i1 + 1;
        int16_t i3 = i2 + 1;

        int16_t xr, yr, ur, vr, xi, yi, ui, vi;

        sum_dif(sp[i0].fr, sp[i1].fr, &xr, &ur);
        sum_dif(sp[i2].fr, sp[i3].fr, &yr, &vi);
        sum_dif(sp[i0].fi, sp[i1].fi, &xi, &ui);
        sum_dif(sp[i3].fi, sp[i2].fi, &yi, &vr);

        sum_dif(ui, vi, &sp[i1].fi, &sp[i3].fi);
        sum_dif(xi, yi, &sp[i0].fi, &sp[i2].fi);
        sum_dif(ur, vr, &sp[i1].fr, &sp[i3].fr);
        sum_dif(xr, yr, &sp[i0].fr, &sp[i2].fr);
    }

    for (ldm = 2 * rdx; ldm <= FFT_LOG2; ldm += rdx) {
        int16_t m = (1 << ldm);
        int16_t m4 = (m >> rdx);

        int16_t phi0 =  N_WAVE / m;
        int16_t phi  = 0;

        for (int16_t i = 0; i < m4; i++) {
            int16_t sin1, sin2, sin3;
            int16_t cos1, cos2, cos3;

            sin1 = fft_sin(1 * phi);
            sin2 = fft_sin(2 * phi);
            sin3 = fft_sin(3 * phi);

            cos1 = fft_cos(1 * phi);
            cos2 = fft_cos(2 * phi);
            cos3 = fft_cos(3 * phi);

            for (int16_t r = 0; r < FFT_SIZE; r += m) {
                int16_t i0 = i + r;
                int16_t i1 = i0 + m4;
                int16_t i2 = i1 + m4;
                int16_t i3 = i2 + m4;

                int16_t xr, yr, ur, vr, xi, yi, ui, vi;
                int16_t t;

                mult_shf(cos2, sin2, sp[i1].fr, sp[i1].fi, &xr, &xi);
                mult_shf(cos1, sin1, sp[i2].fr, sp[i2].fi, &yr, &vr);
                mult_shf(cos3, sin3, sp[i3].fr, sp[i3].fi, &vi, &yi);

                t = yi - vr;
                yi += vr;
                vr = t;

                ur = sp[i0].fr - xr;
                xr += sp[i0].fr;

                sum_dif(ur, vr, &sp[i1].fr, &sp[i3].fr);

                t = yr - vi;
                yr += vi;
                vi = t;

                ui = sp[i0].fi - xi;
                xi += sp[i0].fi;

                sum_dif(ui, vi, &sp[i1].fi, &sp[i3].fi);
                sum_dif(xr, yr, &sp[i0].fr, &sp[i2].fr);
                sum_dif(xi, yi, &sp[i0].fi, &sp[i2].fi);
            }
            phi += phi0;
        }
    }
}

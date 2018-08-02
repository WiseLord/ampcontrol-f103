#include "fft.h"

static const int16_t sinTable[N_WAVE] = {
         0,    402,    803,   1205,   1605,   2005,   2403,   2800,
      3196,   3589,   3980,   4369,   4755,   5139,   5519,   5896,
      6269,   6639,   7004,   7365,   7722,   8075,   8422,   8764,
      9101,   9433,   9759,  10079,  10393,  10700,  11002,  11296,
     11584,  11865,  12139,  12405,  12664,  12915,  13158,  13394,
     13621,  13841,  14052,  14254,  14448,  14633,  14810,  14977,
     15135,  15285,  15425,  15556,  15677,  15789,  15892,  15984,
     16068,  16141,  16205,  16259,  16304,  16338,  16363,  16378,
     16383,  16378,  16363,  16338,  16304,  16259,  16205,  16141,
     16068,  15984,  15892,  15789,  15677,  15556,  15425,  15285,
     15135,  14977,  14810,  14633,  14448,  14254,  14052,  13841,
     13621,  13394,  13158,  12915,  12664,  12405,  12139,  11865,
     11584,  11296,  11002,  10700,  10393,  10079,   9759,   9433,
      9101,   8764,   8422,   8075,   7722,   7365,   7004,   6639,
      6269,   5896,   5519,   5139,   4755,   4369,   3980,   3589,
      3196,   2800,   2403,   2005,   1605,   1205,    803,    402,
         0,   -402,   -803,  -1205,  -1605,  -2005,  -2403,  -2800,
     -3196,  -3589,  -3980,  -4369,  -4755,  -5139,  -5519,  -5896,
     -6269,  -6639,  -7004,  -7365,  -7722,  -8075,  -8422,  -8764,
     -9101,  -9433,  -9759, -10079, -10393, -10700, -11002, -11296,
    -11584, -11865, -12139, -12405, -12664, -12915, -13158, -13394,
    -13621, -13841, -14052, -14254, -14448, -14633, -14810, -14977,
    -15135, -15285, -15425, -15556, -15677, -15789, -15892, -15984,
    -16068, -16141, -16205, -16259, -16304, -16338, -16363, -16378,
    -16383, -16378, -16363, -16338, -16304, -16259, -16205, -16141,
    -16068, -15984, -15892, -15789, -15677, -15556, -15425, -15285,
    -15135, -14977, -14810, -14633, -14448, -14254, -14052, -13841,
    -13621, -13394, -13158, -12915, -12664, -12405, -12139, -11865,
    -11584, -11296, -11002, -10700, -10393, -10079,  -9759,  -9433,
     -9101,  -8764,  -8422,  -8075,  -7722,  -7365,  -7004,  -6639,
     -6269,  -5896,  -5519,  -5139,  -4755,  -4369,  -3980,  -3589,
     -3196,  -2800,  -2403,  -2005,  -1605,  -1205,   -803,   -402,
};

static const int16_t dbTable[N_DB - 1] = {
    1,    1,    2,    2,    3,    4,    6,    8,
    10,   14,   18,   24,   33,   44,   59,   78,
    105,  140,  187,  250,  335,  448,  599,  801,
    1071, 1432, 1915, 2561, 3425, 4580, 6125
};

void rev_bin(int16_t *fr)
{
    int16_t m, mr = 0;
    int16_t tr, l;

    for (m = 1; m < FFT_SIZE; m++) {
        l = FFT_SIZE;
        do
            l >>= 1;
        while (mr + l >= FFT_SIZE);

        mr = (mr & (l - 1)) + l;

        if (mr <= m)
            continue;
        tr = fr[m];
        fr[m] = fr[mr];
        fr[mr] = tr;
    }
    return;
}

static inline void sum_dif(int16_t a, int16_t b, int16_t *s, int16_t *d)
{
    *s = a + b;
    *d = a - b;
}
static inline void mult_shf(int16_t cos, int16_t sin, int16_t x, int16_t y, int16_t *u, int16_t *v)
{
    *u = ((long)x * cos - (long)y * sin) >> 14;
    *v = ((long)y * cos + (long)x * sin) >> 14;
}

void fft_radix4(int16_t *fr, int16_t *fi)
{
    uint8_t ldm = 0, rdx = 2;
    uint16_t i0, i1, i2, i3;
    int16_t xr, yr, ur, vr, xi, yi, ui, vi, t;
    int16_t cos1, sin1, cos2, sin2, cos3, sin3;
    uint16_t m;
    uint8_t m4, phI0, phI;
    uint16_t r, i;

    for (i0 = 0; i0 < FFT_SIZE; i0 += 4) {
        i1 = i0 + 1;
        i2 = i1 + 1;
        i3 = i2 + 1;

        sum_dif(fr[i0], fr[i1], &xr, &ur);
        sum_dif(fr[i2], fr[i3], &yr, &vi);
        sum_dif(fi[i0], fi[i1], &xi, &ui);
        sum_dif(fi[i3], fi[i2], &yi, &vr);

        sum_dif(ui, vi, &fi[i1], &fi[i3]);
        sum_dif(xi, yi, &fi[i0], &fi[i2]);
        sum_dif(ur, vr, &fr[i1], &fr[i3]);
        sum_dif(xr, yr, &fr[i0], &fr[i2]);
    }

    for (ldm = 2 * rdx; ldm <= FFT_LOG2; ldm += rdx) {
        m = (1 << ldm);
        m4 = (m >> rdx);

        phI0 =  N_WAVE / m;
        phI  = 0;

        for (i = 0; i < m4; i++) {
            sin1 = sinTable[phI];
            sin2 = sinTable[2 * phI];
            sin3 = sinTable[3 * phI];

            cos1 = sinTable[phI + N_WAVE / 4];
            cos2 = sinTable[2 * phI + N_WAVE / 4];
            cos3 = sinTable[3 * phI + N_WAVE / 4];

            for (r = 0; r < FFT_SIZE; r += m) {
                i0 = i + r;
                i1 = i0 + m4;
                i2 = i1 + m4;
                i3 = i2 + m4;

                mult_shf(cos2, sin2, fr[i1], fi[i1], &xr, &xi);
                mult_shf(cos1, sin1, fr[i2], fi[i2], &yr, &vr);
                mult_shf(cos3, sin3, fr[i3], fi[i3], &vi, &yi);

                t = yi - vr;
                yi += vr;
                vr = t;

                ur = fr[i0] - xr;
                xr += fr[i0];

                sum_dif(ur, vr, &fr[i1], &fr[i3]);

                t = yr - vi;
                yr += vi;
                vi = t;

                ui = fi[i0] - xi;
                xi += fi[i0];

                sum_dif(ui, vi, &fi[i1], &fi[i3]);
                sum_dif(xr, yr, &fr[i0], &fr[i2]);
                sum_dif(xi, yi, &fi[i0], &fi[i2]);
            }
            phI += phI0;
        }
    }
    return;
}

void cplx2dB(int16_t *fr, int16_t *fi)
{
    uint16_t i, j;
    int16_t calc;
    for (i = 0; i < FFT_SIZE / 2; i++) {
        calc = ((long)fr[i] * fr[i] + (long)fi[i] * fi[i]) >> 13;

        for (j = 0; j < N_DB; j++)
            if (calc <= dbTable[j])
                break;
        fr[i] = j;
    }
    return;
}

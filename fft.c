#include "fft.h"

static const int16_t sinTable[N_WAVE / 2] = {
    0,      804,    1607,   2410,   3211,   4011,   4807,   5601,
    6392,   7179,   7961,   8739,   9511,   10278,  11038,  11792,
    12539,  13278,  14009,  14732,  15446,  16150,  16845,  17530,
    18204,  18867,  19519,  20159,  20787,  21402,  22004,  22594,
    23169,  23731,  24278,  24811,  25329,  25831,  26318,  26789,
    27244,  27683,  28105,  28510,  28897,  29268,  29621,  29955,
    30272,  30571,  30851,  31113,  31356,  31580,  31785,  31970,
    32137,  32284,  32412,  32520,  32609,  32678,  32727,  32757,
    32767,  32757,  32727,  32678,  32609,  32520,  32412,  32284,
    32137,  31970,  31785,  31580,  31356,  31113,  30851,  30571,
    30272,  29955,  29621,  29268,  28897,  28510,  28105,  27683,
    27244,  26789,  26318,  25831,  25329,  24811,  24278,  23731,
    23169,  22594,  22004,  21402,  20787,  20159,  19519,  18867,
    18204,  17530,  16845,  16150,  15446,  14732,  14009,  13278,
    12539,  11792,  11038,  10278,  9511,   8739,   7961,   7179,
    6392,   5601,   4807,   4011,   3211,   2410,   1607,   804,
};

static const uint16_t hammTable[FFT_SIZE / 2] = {
    5027,   5037,   5064,   5110,   5174,   5257,   5357,   5476,
    5613,   5768,   5941,   6132,   6340,   6566,   6810,   7070,
    7348,   7643,   7954,   8283,   8627,   8988,   9365,   9757,
    10165,  10588,  11026,  11479,  11946,  12428,  12923,  13432,
    13954,  14490,  15037,  15597,  16169,  16753,  17348,  17954,
    18570,  19196,  19833,  20478,  21133,  21796,  22467,  23146,
    23833,  24526,  25226,  25932,  26644,  27361,  28083,  28809,
    29539,  30272,  31009,  31748,  32489,  33233,  33977,  34722,
    35467,  36213,  36957,  37701,  38443,  39184,  39922,  40657,
    41389,  42117,  42841,  43560,  44275,  44984,  45687,  46383,
    47073,  47756,  48431,  49099,  49758,  50408,  51049,  51680,
    52301,  52912,  53513,  54102,  54680,  55246,  55800,  56341,
    56870,  57386,  57888,  58376,  58851,  59311,  59757,  60187,
    60603,  61003,  61388,  61756,  62109,  62445,  62765,  63068,
    63355,  63624,  63876,  64111,  64328,  64528,  64709,  64873,
    65019,  65147,  65257,  65349,  65422,  65477,  65514,  65532,
};

static const int16_t dbTable[N_DB - 1] = {
    1,    1,    2,    2,    3,    4,    6,    8,
    10,   14,   18,   24,   33,   44,   59,   78,
    105,  140,  187,  250,  335,  448,  599,  801,
    1071, 1432, 1915, 2561, 3425, 4580, 6125
};

static inline int16_t sinTbl(uint8_t phi)
{
    return sinTable[phi & 0x7F] * (phi < N_WAVE / 2 ? 1 : -1);
}

static inline void sum_dif(int16_t a, int16_t b,
                           int16_t *s, int16_t *d)
{
    *s = a + b;
    *d = a - b;
}

static inline void mult_shf(int16_t cos, int16_t sin, int16_t x, int16_t y,
                            int16_t *u, int16_t *v)
{
    *u = ((int32_t)x * cos - (int32_t)y * sin) >> 15;
    *v = ((int32_t)y * cos + (int32_t)x * sin) >> 15;
}

void fft_hamm_window(int16_t *fr)
{
    for (uint16_t i = 0; i < FFT_SIZE / 2; i++) {
        uint32_t ht = hammTable[i];
        fr[i] = (ht * fr[i]) >> 16;
        fr[FFT_SIZE - 1 - i] = (ht * fr[FFT_SIZE - 1 - i]) >> 16;
    }
}

void fft_rev_bin(int16_t *fr)
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
        tr = fr[m];
        fr[m] = fr[mr];
        fr[mr] = tr;
    }
}

void fft_radix4(int16_t *fr, int16_t *fi)
{
    int16_t ldm = 0, rdx = 2;

    for (int16_t i0 = 0; i0 < FFT_SIZE; i0 += 4) {
        int16_t i1 = i0 + 1;
        int16_t i2 = i1 + 1;
        int16_t i3 = i2 + 1;

        int16_t xr, yr, ur, vr, xi, yi, ui, vi;

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
        int16_t m = (1 << ldm);
        int16_t m4 = (m >> rdx);

        int16_t phi0 =  N_WAVE / m;
        int16_t phi  = 0;

        for (int16_t i = 0; i < m4; i++) {
            int16_t sin1, sin2, sin3;
            int16_t cos1, cos2, cos3;

            sin1 = sinTbl(1 * phi);
            sin2 = sinTbl(2 * phi);
            sin3 = sinTbl(3 * phi);

            cos1 = sinTbl(1 * phi + N_WAVE / 4);
            cos2 = sinTbl(2 * phi + N_WAVE / 4);
            cos3 = sinTbl(3 * phi + N_WAVE / 4);

            for (int16_t r = 0; r < FFT_SIZE; r += m) {
                int16_t i0 = i + r;
                int16_t i1 = i0 + m4;
                int16_t i2 = i1 + m4;
                int16_t i3 = i2 + m4;

                int16_t xr, yr, ur, vr, xi, yi, ui, vi;
                int16_t t;

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
            phi += phi0;
        }
    }
}

void fft_cplx2dB(int16_t *fr, int16_t *fi)
{
    uint16_t i, j;
    int16_t calc;
    for (i = 0; i < FFT_SIZE / 2; i++) {
        calc = ((int32_t)fr[i] * fr[i] + (int32_t)fi[i] * fi[i]) >> 20;

        for (j = 0; j < N_DB; j++)
            if (calc <= dbTable[j])
                break;

        fr[i] = j;
    }
}

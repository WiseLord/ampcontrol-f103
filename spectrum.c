#include "spectrum.h"

#include <stm32f1xx_ll_adc.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_rcc.h>

#include "eemul.h"
#include "functions.h"
#include "display/glcd.h"

#define DMA_BUF_SIZE        (FFT_SIZE * 2)

static Spectrum spectrum;

// Array with ADC data, interleaved L-R-L-R...
static int16_t bufDMA[DMA_BUF_SIZE];
static FftSample *sp; // sp[FFT_SIZE] is shared with glcd unRleImgData

static const uint16_t hammTable[N_HANN / 2] = {
    5027,   5028,   5030,   5032,   5036,   5042,   5048,   5055,
    5064,   5074,   5084,   5096,   5109,   5124,   5139,   5156,
    5173,   5192,   5212,   5233,   5255,   5279,   5303,   5329,
    5355,   5383,   5412,   5442,   5474,   5506,   5539,   5574,
    5610,   5647,   5685,   5724,   5764,   5805,   5848,   5891,
    5936,   5982,   6028,   6076,   6125,   6176,   6227,   6279,
    6333,   6387,   6443,   6499,   6557,   6616,   6676,   6737,
    6799,   6862,   6927,   6992,   7058,   7126,   7194,   7264,
    7335,   7406,   7479,   7553,   7628,   7704,   7781,   7859,
    7938,   8018,   8099,   8181,   8264,   8348,   8433,   8519,
    8606,   8695,   8784,   8874,   8965,   9057,   9151,   9245,
    9340,   9436,   9533,   9631,   9730,   9830,   9931,   10033,
    10136,  10239,  10344,  10450,  10557,  10664,  10773,  10882,
    10992,  11104,  11216,  11329,  11443,  11558,  11673,  11790,
    11907,  12026,  12145,  12265,  12386,  12508,  12631,  12755,
    12879,  13004,  13131,  13258,  13385,  13514,  13643,  13774,
    13905,  14037,  14169,  14303,  14437,  14572,  14708,  14845,
    14982,  15120,  15259,  15399,  15539,  15681,  15822,  15965,
    16108,  16253,  16397,  16543,  16689,  16836,  16984,  17132,
    17281,  17431,  17581,  17732,  17884,  18036,  18189,  18343,
    18497,  18652,  18808,  18964,  19120,  19278,  19436,  19594,
    19754,  19913,  20074,  20235,  20396,  20558,  20721,  20884,
    21048,  21212,  21377,  21542,  21708,  21874,  22041,  22208,
    22376,  22544,  22713,  22882,  23052,  23222,  23393,  23564,
    23736,  23908,  24080,  24253,  24426,  24600,  24774,  24948,
    25123,  25298,  25474,  25650,  25826,  26003,  26180,  26357,
    26535,  26713,  26891,  27070,  27249,  27428,  27608,  27788,
    27968,  28148,  28329,  28510,  28691,  28873,  29054,  29236,
    29419,  29601,  29784,  29966,  30149,  30333,  30516,  30700,
    30883,  31067,  31251,  31436,  31620,  31805,  31989,  32174,
    32359,  32544,  32729,  32914,  33099,  33285,  33470,  33656,
    33841,  34027,  34213,  34398,  34584,  34770,  34956,  35142,
    35327,  35513,  35699,  35885,  36071,  36256,  36442,  36628,
    36813,  36999,  37184,  37370,  37555,  37740,  37925,  38110,
    38295,  38480,  38665,  38850,  39034,  39218,  39402,  39586,
    39770,  39954,  40137,  40321,  40504,  40687,  40870,  41052,
    41234,  41416,  41598,  41780,  41961,  42142,  42323,  42504,
    42684,  42864,  43044,  43223,  43402,  43581,  43760,  43938,
    44116,  44293,  44471,  44647,  44824,  45000,  45176,  45351,
    45526,  45701,  45875,  46049,  46223,  46396,  46568,  46740,
    46912,  47083,  47254,  47425,  47595,  47764,  47933,  48102,
    48270,  48437,  48604,  48771,  48937,  49103,  49268,  49432,
    49596,  49760,  49922,  50085,  50247,  50408,  50568,  50728,
    50888,  51047,  51205,  51363,  51520,  51676,  51832,  51987,
    52142,  52296,  52449,  52602,  52754,  52905,  53056,  53206,
    53355,  53504,  53652,  53799,  53946,  54092,  54237,  54381,
    54525,  54668,  54810,  54952,  55093,  55233,  55372,  55511,
    55648,  55785,  55922,  56057,  56192,  56326,  56459,  56591,
    56723,  56853,  56983,  57112,  57240,  57368,  57494,  57620,
    57745,  57869,  57992,  58115,  58236,  58357,  58476,  58595,
    58713,  58830,  58947,  59062,  59176,  59290,  59402,  59514,
    59625,  59735,  59844,  59952,  60059,  60165,  60270,  60374,
    60478,  60580,  60682,  60782,  60881,  60980,  61078,  61174,
    61270,  61364,  61458,  61551,  61642,  61733,  61823,  61911,
    61999,  62086,  62171,  62256,  62340,  62422,  62504,  62584,
    62664,  62742,  62820,  62896,  62972,  63046,  63119,  63191,
    63263,  63333,  63402,  63470,  63537,  63603,  63667,  63731,
    63794,  63855,  63916,  63975,  64034,  64091,  64147,  64202,
    64256,  64309,  64361,  64412,  64461,  64510,  64557,  64603,
    64649,  64693,  64736,  64777,  64818,  64858,  64896,  64934,
    64970,  65005,  65039,  65072,  65104,  65135,  65164,  65193,
    65220,  65246,  65271,  65295,  65318,  65339,  65360,  65379,
    65398,  65415,  65431,  65445,  65459,  65472,  65483,  65493,
    65502,  65510,  65517,  65523,  65528,  65531,  65533,  65534,
};

static const uint16_t dbTable[N_DB] = {
    1,     2,     3,     4,     5,     6,     7,     8,
    9,     10,    11,    12,    13,    14,    16,    17,
    18,    20,    21,    23,    24,    26,    27,    29,
    31,    33,    35,    36,    38,    40,    42,    45,
    47,    49,    51,    54,    56,    59,    62,    64,
    67,    70,    73,    76,    79,    83,    86,    90,
    93,    97,    101,   105,   109,   113,   117,   122,
    126,   131,   136,   141,   146,   152,   157,   163,
    169,   175,   181,   187,   194,   201,   208,   215,
    223,   230,   238,   246,   255,   264,   273,   282,
    291,   301,   311,   322,   333,   344,   355,   367,
    379,   392,   405,   418,   432,   446,   460,   476,
    491,   507,   524,   541,   558,   576,   595,   614,
    634,   654,   675,   697,   720,   743,   766,   791,
    816,   842,   869,   897,   925,   955,   985,   1017,
    1049,  1082,  1116,  1152,  1188,  1226,  1264,  1304,
    1345,  1388,  1432,  1477,  1523,  1571,  1621,  1671,
    1724,  1778,  1834,  1891,  1951,  2012,  2075,  2140,
    2207,  2276,  2347,  2420,  2496,  2574,  2654,  2737,
    2823,  2911,  3002,  3095,  3192,  3291,  3394,  3500,
    3609,  3721,  3837,  3956,  4080,  4207,  4337,  4472,
    4611,  4755,  4903,  5055,  5212,  5374,  5541,  5713,
    5891,  6073,  6262,  6456,  6657,  6863,  7076,  7296,
    7522,  7756,  7996,  8244,  8500,  8764,  9035,  9316,
    9604,  9902,  10209, 10525, 10852, 11188, 11535, 11892,
    12260, 12640, 13032, 13435, 13852, 14281, 14723, 15179,
    15649, 16133, 16633, 17148, 17679, 18226, 18791, 19372,
    19972, 20590, 21227, 21884, 22562, 23260, 23980, 24722,
    25487, 26276, 27089, 27927, 28791, 29682, 30600, 31547,
    32523, 33529, 34567, 35636, 36738, 37875, 39047, 40254,
    41500, 42783, 44107, 45471, 46877, 48327, 49822, 51363,
    52952, 54589, 56278, 58018, 59812, 61662, 63569, 65535,
};

static void spInitDMA(void)
{
    // DMA controller clock enable
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    // Configure NVIC to enable DMA interruptions
    NVIC_SetPriority(DMA1_Channel1_IRQn, 1); // DMA IRQ lower priority than ADC IRQ
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    // Configure the DMA transfer
    LL_DMA_ConfigTransfer(DMA1,
                          LL_DMA_CHANNEL_1,
                          LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                          LL_DMA_MODE_CIRCULAR              |
                          LL_DMA_PERIPH_NOINCREMENT         |
                          LL_DMA_MEMORY_INCREMENT           |
                          LL_DMA_PDATAALIGN_HALFWORD        |
                          LL_DMA_MDATAALIGN_HALFWORD        |
                          LL_DMA_PRIORITY_HIGH               );

    // Set DMA transfer addresses of source and destination
    LL_DMA_ConfigAddresses(DMA1,
                           LL_DMA_CHANNEL_1,
                           LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                           (uint32_t)&bufDMA,
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    // Set DMA transfer size
    LL_DMA_SetDataLength(DMA1,
                         LL_DMA_CHANNEL_1,
                         DMA_BUF_SIZE);

    // Enable the DMA transfer
    LL_DMA_EnableChannel(DMA1,
                         LL_DMA_CHANNEL_1);
}

static void spInitADC(void)
{
    // Configure ADC clock
    LL_RCC_SetADCClockSource(RCC_CFGR_ADCPRE_DIV6);

    // Enable GPIO Clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    // Configure NVIC to enable ADC1 interruptions
    NVIC_SetPriority(ADC1_IRQn, 0);
    NVIC_EnableIRQ(ADC1_IRQn);

    // Enable ADC clock (core clock)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    // Configure GPIO in analog mode to be used as ADC input
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        // Set ADC conversion data alignment
        LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

        // Set Set ADC sequencers scan mode
        LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);
    }

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        // Set ADC group regular trigger source
        LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

        // Set ADC group regular continuous mode
        LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

        // Set ADC group regular conversion data transfer
        LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

        // Set ADC group regular sequencer length and scan direction
        LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

        // Set ADC group regular sequence: channel on the selected sequence rank
        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_8);
        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_9);
    }

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        /* Set ADC channels sampling time */
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_8, LL_ADC_SAMPLINGTIME_71CYCLES_5);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_9, LL_ADC_SAMPLINGTIME_71CYCLES_5);
    }

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        LL_ADC_Enable(ADC1);

        while (!LL_ADC_IsEnabled(ADC1));

        // Run ADC self calibration
        LL_ADC_StartCalibration(ADC1);
        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
        }
    }
}


static void spApplyHammWindow(FftSample *sp)
{
    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {
        uint16_t ht = hammTable[i * (N_HANN / FFT_SIZE)];
        sp[i].fr = (ht * sp[i].fr) >> 16;
        sp[FFT_SIZE - 1 - i].fr = (ht * sp[FFT_SIZE - 1 - i].fr) >> 16;
    }
}

static inline uint8_t spGetDb(uint16_t value, uint8_t min, uint8_t max)
{
    uint8_t mid = (min + max) / 2;

    if (dbTable[mid] < value) {
        return mid == min ? mid : spGetDb(value, mid, max);
    } else {
        return mid == max ? mid : spGetDb(value, min, mid);
    }
}

static void spCplx2dB(FftSample *sp, uint8_t *out)
{
    int16_t i, j;
    uint16_t accum = 0;

    for (i = 0, j = 0; i < FFT_SIZE / 2; i++) {
        uint16_t calc = (sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi) >> 16;
        accum += spGetDb(calc, 0, N_DB - 1);

        if (i < 48) {
            out[j++] = (uint8_t)(accum / 1);
            accum = 0;
        } else if (i < 96) {
            if ((i & 0x01) == 0x01) {
                out[j++] = (uint8_t)(accum / 2);
                accum = 0;
            }
        } else if (i < 192) {
            if ((i & 0x03) == 0x03) {
                out[j++] = (uint8_t)(accum / 4);
                accum = 0;
            }
        } else if (i < 384) {
            if ((i & 0x07) == 0x07) {
                out[j++] = (uint8_t)(accum / 8);
                accum = 0;
            }
        } else {
            if ((i & 0x0F) == 0x0F) {
                out[j++] = (uint8_t)(accum / 16);
                accum = 0;
            }
        }
    }
}

static void spGetData(int16_t *dma, uint8_t *data)
{
    int32_t dcOft = 0;

    for (int16_t i = 0; i < FFT_SIZE; i++) {
        sp[i].fr = dma[2 * i];
        dcOft += sp[i].fr;
    }
    dcOft /= FFT_SIZE;

    for (int16_t i = 0; i < FFT_SIZE; i++) {
        sp[i].fr -= dcOft;
        sp[i].fi = 0;
    }

    spApplyHammWindow(sp);
    fft_rev_bin(sp);

    fft_radix4(sp);
    spCplx2dB(sp, data);
}

static void spReadSettings(void)
{
    spectrum.mode = (SpMode)(eeReadI(EE_SPECTRUM_MODE, (int16_t)SP_MODE_STEREO));
}

void spInit(void)
{
    spReadSettings();

    sp = (FftSample *)glcdGetUnrleImgData(); // Share working FFT buffer with glcd module

    spInitDMA();
    spInitADC();
}

Spectrum *spGet(void)
{
    return &spectrum;
}

void spGetADC(uint8_t *dataL, uint8_t *dataR)
{
    spGetData((int16_t *)(bufDMA + 0), dataL);
    spGetData((int16_t *)(bufDMA + 1), dataR);
}

void spConvertADC(void)
{
    if (LL_ADC_IsEnabled(ADC1) == 1) {
        LL_ADC_REG_StartConversionSWStart(ADC1);
    }
}

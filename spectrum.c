#include "spectrum.h"

#include <stm32f1xx_ll_adc.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_rcc.h>

#include "eemap.h"
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
    256,   262,   267,   273,   279,   285,   292,   298,
    305,   311,   318,   325,   332,   340,   347,   355,
    363,   370,   379,   387,   395,   404,   413,   422,
    431,   441,   451,   460,   471,   481,   492,   502,
    513,   525,   536,   548,   560,   572,   585,   598,
    611,   624,   638,   652,   666,   681,   696,   711,
    727,   743,   759,   776,   793,   811,   828,   847,
    865,   884,   904,   923,   944,   965,   986,   1007,
    1030,  1052,  1075,  1099,  1123,  1148,  1173,  1199,
    1225,  1252,  1280,  1308,  1337,  1366,  1396,  1427,
    1458,  1490,  1523,  1556,  1591,  1625,  1661,  1698,
    1735,  1773,  1812,  1852,  1893,  1934,  1977,  2020,
    2065,  2110,  2157,  2204,  2252,  2302,  2353,  2404,
    2457,  2511,  2566,  2623,  2680,  2739,  2800,  2861,
    2924,  2988,  3054,  3121,  3190,  3260,  3331,  3405,
    3480,  3556,  3634,  3714,  3796,  3879,  3965,  4052,
    4141,  4232,  4325,  4420,  4517,  4616,  4718,  4822,
    4928,  5036,  5147,  5260,  5375,  5494,  5614,  5738,
    5864,  5993,  6124,  6259,  6397,  6537,  6681,  6828,
    6978,  7131,  7288,  7448,  7612,  7780,  7951,  8125,
    8304,  8487,  8673,  8864,  9059,  9258,  9461,  9669,
    9882,  10099, 10321, 10548, 10780, 11017, 11259, 11507,
    11760, 12018, 12282, 12552, 12828, 13110, 13399, 13693,
    13994, 14302, 14616, 14937, 15266, 15601, 15944, 16295,
    16653, 17019, 17393, 17776, 18167, 18566, 18974, 19391,
    19818, 20253, 20698, 21153, 21619, 22094, 22579, 23076,
    23583, 24102, 24631, 25173, 25726, 26292, 26870, 27461,
    28064, 28681, 29312, 29956, 30615, 31288, 31976, 32679,
    33397, 34131, 34882, 35648, 36432, 37233, 38051, 38888,
    39743, 40617, 41510, 42422, 43355, 44308, 45282, 46277,
    47295, 48334, 49397, 50483, 51593, 52727, 53886, 55071,
    56281, 57519, 58783, 60075, 61396, 62746, 64125, 65535,
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
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);

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
        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
    }

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        /* Set ADC channels sampling time */
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_71CYCLES_5);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_71CYCLES_5);
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
        uint16_t calc = (uint16_t)((sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi) >> 15);

        accum += spGetDb(calc, 0, N_DB - 1);

        if (i < 48) {
            out[j++] = (uint8_t)(accum >> 0);
            accum = 0;
        } else if (i < 96) {
            if ((i & 0x01) == 0x01) {
                out[j++] = (uint8_t)(accum >> 1);
                accum = 0;
            }
        } else if (i < 192) {
            if ((i & 0x03) == 0x03) {
                out[j++] = (uint8_t)(accum >> 2);
                accum = 0;
            }
        } else if (i < 384) {
            if ((i & 0x07) == 0x07) {
                out[j++] = (uint8_t)(accum >> 4);
                accum = 0;
            }
        } else {
            if ((i & 0x0F) == 0x0F) {
                out[j++] = (uint8_t)(accum >> 8);
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
        sp[i].fr >>= 2; // Use only 10 most significant bits for FFT
        sp[i].fi = 0;
    }

    spApplyHammWindow(sp);
    fft_rev_bin(sp);
    fft_radix4(sp);

    spCplx2dB(sp, data);
}

static void spReadSettings(void)
{
    spectrum.mode = (SpMode)(eeRead(EE_SPECTRUM_MODE));
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

void spGetADC(Spectrum *sp)
{
    spGetData((int16_t *)(bufDMA + 0), sp->chan[SP_CHAN_LEFT].raw);
    spGetData((int16_t *)(bufDMA + 1), sp->chan[SP_CHAN_RIGHT].raw);
}

void spConvertADC(void)
{
    if (LL_ADC_IsEnabled(ADC1) == 1) {
        LL_ADC_REG_StartConversionSWStart(ADC1);
    }
}

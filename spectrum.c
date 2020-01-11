#include "spectrum.h"

#include "hwlibs.h"

#include <stdlib.h>
#include <string.h>

#include "fft.h"
#include "settings.h"
#include "utils.h"

#define DMA_BUF_SIZE        (FFT_SIZE * SP_CHAN_END)

static Spectrum spectrum;

typedef struct {
    int16_t chan[SP_CHAN_END];
} SpDataSet;

typedef union {
    SpDataSet dataSet[FFT_SIZE];
    int16_t bufDMA[DMA_BUF_SIZE];
} SpDMAData;

static SpDMAData dmaData;

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
                           (uint32_t)&dmaData,
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
#ifdef STM32F1
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);
#endif
#ifdef STM32F3
    LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSRC_PLL_DIV_6);
#endif

    // Configure NVIC to enable ADC1 interruptions
    NVIC_SetPriority(ADC1_IRQn, 0);
    NVIC_EnableIRQ(ADC1_IRQn);

    // Enable ADC clock (core clock)
#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
#endif
#ifdef STM32F3
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);
#endif

    // Configure GPIO in analog mode to be used as ADC input
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
#ifdef STM32F3
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);
#endif

    if (!LL_ADC_IsEnabled(ADC1)) {
        // Set ADC conversion data alignment
        LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

        // Set Set ADC sequencers scan mode
#ifdef STM32F1
        LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);
#endif

        // Set ADC group regular trigger source
        LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

        // Set ADC group regular continuous mode
        LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

        // Set ADC group regular conversion data transfer
        LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

        // Set ADC group regular sequencer length and scan direction
        LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

#ifdef STM32F1
        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_71CYCLES_5);

        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_71CYCLES_5);
#endif
#ifdef STM32F3
        LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_61CYCLES_5);
//        LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SINGLE_ENDED); // bug in the function
        ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_1;

        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_2);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_61CYCLES_5);
//        LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SINGLE_ENDED); // bug in the function
        ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_2;
#endif

#ifdef STM32F1
        LL_ADC_Enable(ADC1);
        while (!LL_ADC_IsEnabled(ADC1));

        LL_ADC_StartCalibration(ADC1);
        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0);
#endif
#ifdef STM32F3
        LL_ADC_EnableInternalRegulator(ADC1);
        while (!LL_ADC_IsInternalRegulatorEnabled(ADC1));

        LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

        utilmDelay(1);

        LL_ADC_Enable(ADC1);
        while (!LL_ADC_IsEnabled(ADC1));
#endif
    }
}


static void spApplyHammWindow(FftSample *sp)
{
    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {

        // Use Hamm coefficients 0.53836 and 0.46164 scaled by SIN_SCALE = 2^15
        uint16_t ht = (uint16_t)(35281 - ((30253 * fft_cos(i * (N_HANN / FFT_SIZE))) >> 15));

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
    uint8_t db;
    uint8_t *po = out;

    memset(po, 0, SPECTRUM_SIZE);

    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {
        uint16_t calc = (uint16_t)((sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi) >> 15);

        db = spGetDb(calc, 0, N_DB - 1);

        if (*po < db) {
            *po = db;
        }

        if ((i < 48) ||
            ((i < 96) && (i & 0x01) == 0x01) ||
            ((i < 192) && (i & 0x03) == 0x03) ||
            ((i < 384) && (i & 0x07) == 0x07) ||
            ((i & 0x0F) == 0x0F)) {
            po++;
        }
    }
}

static void spGetData(int16_t *dma, SpData *chan)
{
    int32_t dcOft = 0;

    FftSample *sp = malloc(sizeof (FftSample) * FFT_SIZE);

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

    spCplx2dB(sp, chan->raw);

    int16_t total = 0;
    chan->max = 0;
    for (int16_t i = 0; i < SPECTRUM_SIZE; i++) {
        uint8_t raw = chan->raw[i];
        total += raw;
        if (chan->max < raw) {
            chan->max = raw;
        }
    }
    chan->avg = (uint8_t)(total / SPECTRUM_SIZE);

    free(sp);
}

static void spReadSettings(void)
{
    spectrum.mode = (SpMode)settingsRead(PARAM_SPECTRUM_MODE);
    spectrum.peaks = (uint8_t)settingsRead(PARAM_SPECTRUM_PEAKS);
    spectrum.grad = (uint8_t)settingsRead(PARAM_SPECTRUM_GRAD);
}

void spInit(void)
{
    spReadSettings();

    spInitDMA();
    spInitADC();
}

Spectrum *spGet(void)
{
    return &spectrum;
}

void spGetADC(Spectrum *sp)
{
    for (SpChan chan = SP_CHAN_LEFT; chan < SP_CHAN_BOTH; chan++) {
        spGetData(&dmaData.dataSet->chan[chan], &sp->data[chan]);
    }
}

void spConvertADC(void)
{
    if (LL_ADC_IsEnabled(ADC1) == 1) {
#ifdef STM32F1
        LL_ADC_REG_StartConversionSWStart(ADC1);
#endif
#ifdef STM32F3
        LL_ADC_REG_StartConversion(ADC1);
#endif
    }
}

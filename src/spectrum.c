#include "spectrum.h"

#include "hwlibs.h"

#ifdef _USE_SETTINGS
#include "settings.h"
#else
#define settingsRead(param, value) value
#endif

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
    640,   646,   651,   657,   663,   669,   676,   682,
    689,   695,   702,   709,   715,   723,   730,   738,
    746,   753,   762,   770,   778,   787,   796,   805,
    813,   823,   833,   842,   853,   863,   874,   884,
    896,   908,   918,   930,   942,   954,   967,   980,
    993,   1006,  1019,  1033,  1047,  1062,  1077,  1092,
    1108,  1123,  1139,  1157,  1174,  1192,  1209,  1227,
    1245,  1264,  1284,  1303,  1323,  1344,  1365,  1386,
    1410,  1431,  1454,  1478,  1502,  1526,  1551,  1577,
    1603,  1629,  1658,  1686,  1715,  1743,  1773,  1804,
    1834,  1866,  1899,  1932,  1967,  2001,  2036,  2073,
    2110,  2147,  2187,  2227,  2267,  2308,  2350,  2393,
    2438,  2483,  2530,  2576,  2624,  2674,  2725,  2775,
    2828,  2881,  2937,  2993,  3049,  3108,  3169,  3230,
    3292,  3355,  3422,  3488,  3556,  3626,  3697,  3770,
    3845,  3920,  3998,  4077,  4158,  4242,  4327,  4413,
    4502,  4592,  4685,  4779,  4875,  4974,  5075,  5178,
    5284,  5391,  5502,  5614,  5729,  5847,  5967,  6089,
    6215,  6343,  6474,  6607,  6745,  6884,  7027,  7173,
    7322,  7475,  7630,  7790,  7952,  8119,  8290,  8462,
    8640,  8822,  9008,  9197,  9391,  9589,  9791,  9998,
    10209, 10425, 10645, 10871, 11102, 11338, 11578, 11825,
    12076, 12333, 12595, 12863, 13138, 13418, 13705, 13997,
    14297, 14603, 14915, 15234, 15562, 15895, 16235, 16585,
    16940, 17304, 17676, 18056, 18445, 18842, 19247, 19662,
    20087, 20519, 20962, 21414, 21878, 22349, 22831, 23325,
    23829, 24345, 24871, 25411, 25960, 26523, 27097, 27685,
    28284, 28897, 29525, 30165, 30820, 31490, 32173, 32872,
    33586, 34316, 35062, 35824, 36603, 37399, 38212, 39045,
    39895, 40763, 41651, 42558, 43485, 44433, 45401, 46391,
    47403, 48435, 49492, 50571, 51675, 52802, 53955, 55132,
    56335, 57566, 58823, 60107, 61420, 62763, 64133, 65535,
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
        LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SINGLE_ENDED);

        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_2);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_61CYCLES_5);
        LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SINGLE_ENDED);
#endif

#ifdef STM32F1
        LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD_ALL_CHANNELS_REG);
        LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD_THRESHOLD_LOW, 2047 - 512);
        LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD_THRESHOLD_HIGH, 2047 + 512);
#endif

#ifdef STM32F3
        LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD1, LL_ADC_AWD_ALL_CHANNELS_REG);
        LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD1, LL_ADC_AWD_THRESHOLD_LOW, 2047 - 512);
        LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD1, LL_ADC_AWD_THRESHOLD_HIGH, 2047 + 512);
#endif

#ifdef STM32F3
        LL_ADC_SetResolution(ADC2, LL_ADC_RESOLUTION_12B);
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

        LL_mDelay(1);

        LL_ADC_Enable(ADC1);
        while (!LL_ADC_IsEnabled(ADC1));
#endif
    }
}

static void spDoFft(int16_t *dma, FftSample *smpl)
{
    int32_t dcOft = 0;

    for (int16_t i = 0; i < FFT_SIZE; i++) {
        smpl[i].fr = dma[2 * i];
        dcOft += smpl[i].fr;
    }
    dcOft /= FFT_SIZE;

    for (int16_t i = 0; i < FFT_SIZE; i++) {
        smpl[i].fr -= dcOft;
        smpl[i].fr >>= 2; // Use only 10 most significant bits for FFT
        smpl[i].fi = 0;
    }

    fft_hamm_window(smpl);
    fft_rev_bin(smpl);
    fft_radix4(smpl);
}

static void spReadSettings(void)
{
    spectrum.mode = (SpMode)settingsRead(PARAM_SPECTRUM_MODE, SP_MODE_LEFT);
    bool peaks = (uint8_t)settingsRead(PARAM_SPECTRUM_PEAKS, true);
    bool grad = (uint8_t)settingsRead(PARAM_SPECTRUM_GRAD, false);
    bool demo = false;
    spectrum.flags |= ((peaks ? SP_FLAG_PEAKS : SP_FLAG_NONE) |
                       (grad ? SP_FLAG_GRAD : SP_FLAG_NONE) |
                       (demo ? SP_FLAG_DEMO : SP_FLAG_NONE));
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

uint8_t spGetDb(uint16_t value)
{
    uint8_t min = 0;
    uint8_t max = N_DB - 1;

    uint8_t mid = (min + max) / 2;

    while (min != mid && max != mid) {
        if (dbTable[mid] < value) {
            min = mid;
        } else {
            max = mid;
        }
        mid = (min + max) / 2;
    }

    return mid;
}

void spGetADC(SpChan chan, uint8_t *out, size_t size, fftGet fn)
{
    int16_t *dma = &dmaData.dataSet->chan[chan];

    FftSample smpl[FFT_SIZE];

    spDoFft(dma, smpl);

    if (NULL != fn) {
        fn(smpl, out, size);
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

bool spCheckSignal()
{
    bool ret = LL_ADC_IsActiveFlag_AWD1(ADC1);

    LL_ADC_ClearFlag_AWD1(ADC1);

    return ret;
}

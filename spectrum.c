#include "spectrum.h"

#include <stm32f1xx_ll_adc.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_rcc.h>

#include "fft.h"

#define ADC_DELAY_ENABLE_CALIB_CPU_CYCLES  (LL_ADC_DELAY_ENABLE_CALIB_ADC_CYCLES * 32)

#define FFT_SIZE    64
#define DMA_BUF_SIZE        (FFT_SIZE * 2)

static volatile int16_t bufDMA[DMA_BUF_SIZE];

static int16_t fr[FFT_SIZE];
static int16_t fi[FFT_SIZE];

static uint8_t outL[FFT_SIZE / 2];
static uint8_t outR[FFT_SIZE / 2];

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

    // Enable DMA transfer interruption: transfer complete
    LL_DMA_EnableIT_TC(DMA1,
                       LL_DMA_CHANNEL_1);

    // Enable the DMA transfer
    LL_DMA_EnableChannel(DMA1,
                         LL_DMA_CHANNEL_1);
}

static void spInitADC(void)
{
    // Configure ADC clock
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);

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
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_8, LL_ADC_SAMPLINGTIME_41CYCLES_5);
        LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_9, LL_ADC_SAMPLINGTIME_41CYCLES_5);
    }

    __IO uint32_t wait_loop_index = 0;

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        LL_ADC_Enable(ADC1);

        wait_loop_index = (ADC_DELAY_ENABLE_CALIB_CPU_CYCLES >> 1);
        while (wait_loop_index != 0) {
            wait_loop_index--;
        }

        /* Run ADC self calibration */
        LL_ADC_StartCalibration(ADC1);
        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
        }

    }
}

void spInit()
{
    spInitDMA();

    spInitADC();
}

static const uint8_t hannTable[] = {
    0,   1,   3,   6,  10,  16,  22,  30,
    38,  48,  58,  69,  81,  93, 105, 118,
    131, 143, 156, 168, 180, 191, 202, 212,
    221, 229, 236, 242, 247, 251, 254, 255,
};

#define N_DB        32

static const int16_t dbTable[N_DB - 1] = {
    1,    1,    2,    2,    3,    4,    6,    8,
    10,   14,   18,   24,   33,   44,   59,   78,
    105,  140,  187,  250,  335,  448,  599,  801,
    1071, 1432, 1915, 2561, 3425, 4580, 6125
};

static uint8_t revBits(uint8_t x)
{
    x = ((x & 0x15) << 1) | ((x & 0x2A) >> 1);              // 00abcdef => 00badcfe
    x = (x & 0x0C) | ((x & 0x03) << 4) | ((x & 0x30) >> 4); // 00badcfe => 00fedcba

    return x;
}

static void prepareData()
{
    uint8_t i, j;
    int16_t dcOft = 0;
    uint8_t hw;

    // Calculate average DC offset
    for (i = 0; i < FFT_SIZE; i++)
        dcOft += fi[i];
    dcOft /= FFT_SIZE;

    // Move FI => FR with reversing bit order in index
    for (i = 0; i < FFT_SIZE; i++) {
        j = revBits(i);
        hw = hannTable[i < 32 ? i : 63 - i];
        fr[j] = ((fi[i] - dcOft) * hw) >> 6;
        fi[i] = 0;
    }
}

static void cplx2dB(int16_t *fr, int16_t *fi)
{
    uint8_t i, j;
    int16_t calc;

    for (i = 0; i < FFT_SIZE / 2; i++) {
        calc = ((int32_t)fr[i] * fr[i] + (int32_t)fi[i] * fi[i]) >> 13;

        for (j = 0; j < N_DB - 1; j++)
            if (calc <= dbTable[j])
                break;
        fr[i] = j;
    }
}

void spGetADC(uint8_t **dataL, uint8_t **dataR)
{
    volatile int16_t *dma;

    dma = bufDMA;

    for (int16_t i = 0; i < FFT_SIZE; i++) {
        fi[i] = dma[2 * i] >> 4;
    }

    prepareData();
    fftRad4(fr, fi);
    cplx2dB(fr, fi);

    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {
        outL[i] = fr[i];
    }

    for (int16_t i = 0; i < FFT_SIZE; i++) {
        fi[i] = dma[2 * i + 1] >> 4;
    }

    prepareData();
    fftRad4(fr, fi);
    cplx2dB(fr, fi);

    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {
        outR[i] = fr[i];
    }

    *dataL = outL;
    *dataR = outR;
}

void spConvertADC()
{
    if (LL_ADC_IsEnabled(ADC1) == 1) {
        LL_ADC_REG_StartConversionSWStart(ADC1);
    }
}

void spUpdate()
{
}

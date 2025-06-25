#include "r2s15902.h"

#include "audio.h"
#include "hwlibs.h"
#include "utils.h"

#define R2S15902_SLOTS      4

#define R2S15902_SLOT1_INPUT_SELECTOR   0b111000000000000000000000
#define R2S15902_SLOT1_REC_OUT          0b000100000000000000000000
#define R2S15902_SLOT1_REC_OUT_GAIN     0b000011000000000000000000
#define R2S15902_SLOT1_ADC_INPUT_ATT    0b000000110000000000000000
#define R2S15902_SLOT1_L_R_INPUT        0b000000001000000000000000
#define R2S15902_SLOT1_BASS_TONE_BYPASS 0b000000000111100000000000
#define R2S15902_SLOT1_TREBLE           0b000000000000011110000000
#define R2S15902_SLOT1_SL_SR_C_SW_IN    0b000000000000000001000000
#define R2S15902_SLOT1_INPUT_GAIN       0b000000000000000000111000

#define R2S15902_SLOT2_LCH_GAIN         0b111000000000000000000000
#define R2S15902_SLOT2_LCH_VOLUME       0b000111111100000000000000
#define R2S15902_SLOT2_RCH_GAIN         0b000000000011100000000000
#define R2S15902_SLOT2_RCH_VOLUME       0b000000000000011111110000

#define R2S15902_SLOT3_CCH_GAIN         0b111000000000000000000000
#define R2S15902_SLOT3_CCH_VOLUME       0b000111111100000000000000
#define R2S15902_SLOT3_SWCH_GAIN        0b000000000011100000000000
#define R2S15902_SLOT3_SWCH_VOLUME      0b000000000000011111110000

#define R2S15902_SLOT4_SLCH_GAIN        0b111000000000000000000000
#define R2S15902_SLOT4_SLCH_VOLUME      0b000111111100000000000000
#define R2S15902_SLOT4_SRCH_GAIN        0b000000000011100000000000
#define R2S15902_SLOT4_SRCH_VOLUME      0b000000000000011111110000

#define SPISW_DI_Port                   GPIOA
#define SPISW_DI_Pin                    LL_GPIO_PIN_7
#define SPISW_CLK_Port                  GPIOA
#define SPISW_CLK_Pin                   LL_GPIO_PIN_5

static const AudioGrid gridVolume     = {NULL, -99,  0, (int8_t)(1.00 * STEP_MULT)}; // -99..0dB with 1dB step
static const AudioGrid gridGain       = {NULL,   0,  7, (int8_t)(2.00 * STEP_MULT)}; // 0..14dB with 2dB step
static const AudioGrid gridTone       = {NULL,  -7,  7, (int8_t)(2.00 * STEP_MULT)}; // -14..14dB with 2dB step
static const AudioGrid gridBalance    = {NULL, -15, 15, (int8_t)(1.00 * STEP_MULT)}; // -15..15dB with 1dB step
static const AudioGrid gridSubwoofer  = {NULL, 0, 15, (int8_t)(1.00 * STEP_MULT)}; // -15..15dB with 1dB step

static uint32_t slot[R2S15902_SLOTS] = {
    0x000000,
    0x000001,
    0x000002,
    0x000003,
};

static const AudioApi r2s15092Api = {
    .init = r2s15902Init,
    .getInCnt = r2s15902GetInCnt,

    .setTune = r2s15902SetTune,
    .setInput = r2s15902SetInput,
    .setMute = r2s15902SetMute,
};

static void r2s15904spSendSlot(uint32_t data)
{
    uint8_t i;

    for (i = 0; i < 24; i++) {
        // Set data
        if (data & 0x800000) {
            SET(SPISW_DI);
        } else {
            CLR(SPISW_DI);
        }
        utiluDelay(4);
        // Set clock
        SET(SPISW_CLK);
        utiluDelay(4);
        // Clear data but last bit
        if (i == 23) {
            SET(SPISW_DI);
        } else {
            CLR(SPISW_DI);
        }
        utiluDelay(4);
        // Clear clock
        CLR(SPISW_CLK);
        utiluDelay(4);

        data <<= 1;
    }
    // Latch data
    CLR(SPISW_DI);
    utiluDelay(4);
    // Set clock
    SET(SPISW_CLK);
    utiluDelay(4);
    CLR(SPISW_CLK);
    utiluDelay(4);

    utiluDelay(100);
}


const AudioApi *r2s15902GetApi()
{
    return &r2s15092Api;
}

void r2s15902Init(AudioParam *param)
{
    aPar = param;

    aPar->grid[AUDIO_TUNE_VOLUME]  = &gridVolume;
    aPar->grid[AUDIO_TUNE_BASS]    = &gridTone;
    aPar->grid[AUDIO_TUNE_TREBLE]  = &gridTone;
    aPar->grid[AUDIO_TUNE_GAIN]    = &gridGain;
    aPar->grid[AUDIO_TUNE_BALANCE] = &gridBalance;
    aPar->grid[AUDIO_TUNE_FRONTREAR] = &gridBalance;
    aPar->grid[AUDIO_TUNE_SUBWOOFER] = &gridSubwoofer;
    aPar->grid[AUDIO_TUNE_CENTER] = &gridSubwoofer;

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = SPISW_DI_Pin;
    LL_GPIO_Init(SPISW_DI_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = SPISW_CLK_Pin;
    LL_GPIO_Init(SPISW_CLK_Port, &GPIO_InitStruct);
}

int8_t r2s15902GetInCnt()
{
    return 5;
}

void r2s15902SetTune(AudioTune tune, int8_t value)
{
    AudioRaw raw;
    int8_t val;

    audioSetRawBalance(&raw, aPar->tune[AUDIO_TUNE_VOLUME], false);

    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_GAIN:
        val = aPar->tune[AUDIO_TUNE_GAIN];
        slot[1] = (val << 21) |(-raw.frontLeft << 14) | (val << 11) | (-raw.frontRight << 4) | 0x000001;
        slot[2] = (val << 21) |(-raw.center << 14) | (val << 11) | (-raw.subwoofer << 4) | 0x000002;
        slot[3] = (val << 21) |(-raw.rearLeft << 14) | (val << 11) | (-raw.rearRight << 4) | 0x000003;
        r2s15904spSendSlot(slot[1]);
        r2s15904spSendSlot(slot[2]);
        r2s15904spSendSlot(slot[3]);
        break;
    case AUDIO_TUNE_BASS:
        val = aPar->tune[AUDIO_TUNE_BASS];
        val = val >= 0 ? val + 8 : -val;
        slot[0] &= ~(R2S15902_SLOT1_BASS_TONE_BYPASS);
        slot[0] |= ((uint8_t)val << 11);
        r2s15904spSendSlot(slot[0]);
        break;
    case AUDIO_TUNE_TREBLE:
        val = aPar->tune[AUDIO_TUNE_BASS];
        val = val >= 0 ? val + 8 : -val;
        slot[0] &= ~(R2S15902_SLOT1_TREBLE);
        slot[0] |= ((uint8_t)val << 7);
        r2s15904spSendSlot(slot[0]);
        break;
    default:
        break;
    }
}

void r2s15902SetInput(int8_t value)
{
    switch (value) {
    case 0:
        slot[0] &= ~R2S15902_SLOT1_INPUT_SELECTOR;
        slot[0] |= (R2S15902_SLOT1_L_R_INPUT | R2S15902_SLOT1_SL_SR_C_SW_IN);
        r2s15904spSendSlot(slot[0]);
        break;
    case 1:
        slot[0] &= ~(R2S15902_SLOT1_L_R_INPUT | R2S15902_SLOT1_SL_SR_C_SW_IN);
        slot[0] &= ~R2S15902_SLOT1_INPUT_SELECTOR;
        slot[0] |= (R2S15902_SLOT1_INPUT_SELECTOR & (0x2 << 21));
        r2s15904spSendSlot(slot[0]);
        break;
    case 2:
        slot[0] &= ~(R2S15902_SLOT1_L_R_INPUT | R2S15902_SLOT1_SL_SR_C_SW_IN);
        slot[0] &= ~R2S15902_SLOT1_INPUT_SELECTOR;
        slot[0] |= (R2S15902_SLOT1_INPUT_SELECTOR & (0x4 << 21));
        r2s15904spSendSlot(slot[0]);
        break;
    case 3:
        slot[0] &= ~(R2S15902_SLOT1_L_R_INPUT | R2S15902_SLOT1_SL_SR_C_SW_IN);
        slot[0] &= ~R2S15902_SLOT1_INPUT_SELECTOR;
        slot[0] |= (R2S15902_SLOT1_INPUT_SELECTOR & (0x6 << 21));
        r2s15904spSendSlot(slot[0]);
        break;
    case 4:
        slot[0] &= ~(R2S15902_SLOT1_L_R_INPUT | R2S15902_SLOT1_SL_SR_C_SW_IN);
        slot[0] &= ~R2S15902_SLOT1_INPUT_SELECTOR;
        slot[0] |= (R2S15902_SLOT1_INPUT_SELECTOR & (0x1 << 21));
        r2s15904spSendSlot(slot[0]);
        break;
    default:
        break;
    }
}

void r2s15902SetMute(bool value)
{
    if (value) {
        aPar->tune[AUDIO_TUNE_VOLUME] = gridVolume.min;
        r2s15902SetTune(AUDIO_TUNE_VOLUME, gridVolume.min);
    }
}

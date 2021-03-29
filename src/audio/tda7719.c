#include "tda7719.h"

#include "audio.h"
#include "hwlibs.h"
#include "i2c.h"

// I2C address
#define TDA7719_I2C_ADDR            0x88

// Subaddress
#define TDA7719_TEST_MODE           0x80
#define TDA7719_AUTOINC             0x20
// Function selection
#define TDA7719_INPUT_CONFIG        0x00
#define TDA7719_2ND_SOURCE_DIRECT   0x01
#define TDA7719_MIX_SOURCE_GAIN     0x02
#define TDA7719_MIX_CTRL_LM_DCOFT   0x03
#define TDA7719_SOFT_MUTE           0x04
#define TDA7719_SOFT_STEP_1         0x05
#define TDA7719_SOFT_STEP_2_DET     0x06
#define TDA7719_LOUDNESS            0x07
#define TDA7719_VOLUME_OUTGAIN      0x08
#define TDA7719_TREBLE              0x09
#define TDA7719_MIDDLE              0x0A
#define TDA7719_BASS                0x0B
#define TDA7719_SUB_MID_BASS        0x0C
#define TDA7719_SP_LEFT_FRONT       0x0D
#define TDA7719_SP_RIGHT_FRONT      0x0E
#define TDA7719_SP_LEFT_REAR        0x0F
#define TDA7719_SP_RIGHT_REAR       0x10
#define TDA7719_SUB_LEFT            0x11
#define TDA7719_SUB_RIGHT           0x12
#define TDA7719_TEST_1              0x13
#define TDA7719_TEST_2              0x14

// 0: Input configuration / main selector
#define TDA7719_INPUT_CFG0          0x00
#define TDA7719_INPUT_CFG1          0x20
#define TDA7719_INPUT_CFG2          0x40
#define TDA7719_INPUT_CFG3          0x60
#define TDA7719_INPUT_CFG4          0x80
#define TDA7719_INPUT_CFG5          0xA0
#define TDA7719_INPUT_CFG6          0xC0
#define TDA7719_INPUT_CFG7          0xE0

#define TDA7719_INPUT_GAIN_3DB      0x10
#define TDA7719_INPUT_MD2           0x08

// 1: 2nd source selector / direct path
#define TDA7719_QD4_BYPASS_SUB      0x80
#define TDA7719_QD3_BYPASS_REAR     0x40
#define TDA7719_QD2_BYPASS_FRONT    0x20
#define TDA7719_2ND_IN_GAIN_3DB     0x10
#define TDA7719_2ND_IN_MD2          0x08

// 2: Mising source / mixing gain
#define TDA7719_MIX_ATT_MASK        0xF8

// 3: Mix control / level meter / dc offset detector
#define TDA7719_DC_OFT_DETECTOR     0x80
#define TDA7719_LM_RESET            0x40
#define TDA7719_REF_OUT_EXT         0x20
#define TDA7719_REAR_MAIN_IN        0x10
#define TDA7719_MIX_TO_RR           0x08
#define TDA7719_MIX_TO_RL           0x04
#define TDA7719_MIX_TO_FR           0x02
#define TDA7719_MIX_TO_FL           0x01

// 4: Soft mute / others
#define TDA7719_BYPASS_ANTI_ALIAS   0x80
#define TDA7719_FAST_CHARGE_OFF     0x40
#define TDA7719_SUB_DISABLE         0x20
#define TDA7719_SUB_CONFIG_BASS     0x10
#define TDA7719_SM_TIME_MASK        0x0C
#define TDA7719_MUTE_IIC_ONLY       0x02
#define TDA7719_SM_OFF              0x01

// 5: SoftStep 1
#define TDA7719_SS_LR_OFF           0x80
#define TDA7719_SS_RF_OFF           0x40
#define TDA7719_SS_LF_OFF           0x20
#define TDA7719_SS_BASS_OFF         0x10
#define TDA7719_SS_MIDDLE_OFF       0x08
#define TDA7719_SS_TREBLE_OFF       0x04
#define TDA7719_SS_VOLUME_OFF       0x02
#define TDA7719_SS_LOUDNESS_OFF     0x01

// 6: SoftStep 2 / DC detector
#define TDA7719_SPIKE_REJ_TIME_MASK 0xC0
#define TDA7719_ZERO_COMP_WS_MASK   0x30
#define TDA7719_SS_TIME_10MS        0x08
#define TDA7719_SS_SUB_R_OFF        0x04
#define TDA7719_SS_SUB_L_OFF        0x02
#define TDA7719_SS_RR_OFF           0x01

// 7: Loudness
#define TDA7719_LOUDNESS_ATT_OFT    0
#define TDA7719_LOUD_FREQ_OFT       4

#define TDA7719_HIGH_BOOST_OFF      0x40
#define TDA7719_LOUD_FREQ_FLAT      0x00
#define TDA7719_LOUD_FREQ_400HZ     0x10
#define TDA7719_LOUD_FREQ_800HZ     0x20
#define TDA7719_LOUD_FREQ_2400HZ    0x30
#define TDA7719_LOUD_ATT_MASK       0x0F

// 8: Volume / output gain
#define TDA7719_OUT_GAIN_0DB        0x40
#define TDA7719_GAIN_MASK           0x1F

// 9: Treble filter
#define TDA7719_TREBLE_ATT_OFT      0
#define TDA7719_TREBLE_FREQ_OFT     5

#define TDA7719_TREBLE_FREQ_10K0    0x00
#define TDA7719_TREBLE_FREQ_12K5    0x20
#define TDA7719_TREBLE_FREQ_15K5    0x40
#define TDA7719_TREBLE_FREQ_17K5    0x60
#define TDA7719_TREBLE_GAIN_MASK    0x1F

// 10: Middle filter
#define TDA7719_MIDDLE_ATT_OFT      0
#define TDA7719_MIDDLE_QFACT_OFT    5

#define TDA7719_MIDDLE_QFACT_0P5    0x00
#define TDA7719_MIDDLE_QFACT_0P75   0x20
#define TDA7719_MIDDLE_QFACT_1P0    0x40
#define TDA7719_MIDDLE_QFACT_1P25   0x60
#define TDA7719_MIDDLE_GAIN_MASK    0x1F

// 11: Bass filter
#define TDA7719_BASS_ATT_OFT        0
#define TDA7719_BASS_QFACT_OFT      5

#define TDA7719_BASS_QFACT_1P0      0x00
#define TDA7719_BASS_QFACT_1P25     0x20
#define TDA7719_BASS_QFACT_1P5      0x40
#define TDA7719_BASS_QFACT_2P0      0x60

#define TDA7719_BASS_GAIN_MASK      0x1F

// 12: Subwoofer / middle / bass
#define TDA7719_BASS_DCMODE_OFF_OFT 7
#define TDA7719_BASS_FREQ_OFT       5
#define TDA7719_MIDDLE_FREQ_OFT     3
#define TDA7719_SUB_PHASE_0_OFT     2
#define TDA7719_SUB_CUT_FREQ_OFT    0

#define TDA7719_BASS_DCMODE_OFF     0x80
#define TDA7719_BASS_FREQ_60HZ      0x00
#define TDA7719_BASS_FREQ_80HZ      0x20
#define TDA7719_BASS_FREQ_100HZ     0x40
#define TDA7719_BASS_FREQ_200HZ     0x60
#define TDA7719_MIDDLE_FREQ_500HZ   0x18
#define TDA7719_MIDDLE_FREQ_1000HZ  0x18
#define TDA7719_MIDDLE_FREQ_1500HZ  0x18
#define TDA7719_MIDDLE_FREQ_2000HZ  0x18
#define TDA7719_SUB_PHASE_0         0x04
#define TDA7719_SUB_CUT_FREQ_MASK   0x03

// 13..16: Speaker attenuation
#define TDA7719_SP_ATT_MASK         0x7F

// 17..18: Subwoofer L/R attenuation
#define TDA7719_SUB_ATT_MASK        0x7F

// Common masks
#define TDA7719_INPUT_MASK          0x07
#define TDA7719_SOFT_STEP           0x80

#define TDA7719_INPUT_CFG           TDA7719_INPUT_CFG2

static const int8_t inCfg2[TDA7719_IN_CNT] = { 0, 4, 1, 2, 5, 6 };

static const AudioGrid gridVolume    = {NULL, -79,  0, (int8_t)(1.00 * STEP_MULT)}; // -79..0dB with 1dB step
static const AudioGrid gridToneBal   = {NULL, -15, 15, (int8_t)(1.00 * STEP_MULT)}; // -15..15dB with 1dB step
static const AudioGrid gridSubwoofer = {NULL,   0, 15, (int8_t)(1.00 * STEP_MULT)}; // 0..15dB with 1dB step
static const AudioGrid gridGain      = {NULL,   0,  1, (int8_t)(3.00 * STEP_MULT)}; // 0..3dB with 3dB step
static const AudioGrid gridLoudness  = {NULL, -15,  0, (int8_t)(1.00 * STEP_MULT)}; // -15..0dB with 1dB step

static const int16_t arrBassCFreq[] = {
    60  * STEP_MULT,
    80  * STEP_MULT,
    100 * STEP_MULT,
    200 * STEP_MULT
};
static const AudioGrid adjustBassCFreq = {arrBassCFreq, 0, 3, 0};

static const int16_t arrBassQFact[] = {
    1    * STEP_MULT,
    1.25 * STEP_MULT,
    1.5  * STEP_MULT,
    2    * STEP_MULT
};
static const AudioGrid adjustBassQFact = {arrBassQFact, 0, 3, 0};

static const int16_t arrMiddleCFreqK[] = {
    0.5  * STEP_MULT,
    1    * STEP_MULT,
    1.5 * STEP_MULT,
    2.5 * STEP_MULT
};
static const AudioGrid adjustMiddleCFreqK = {arrMiddleCFreqK, 0, 3, 0};

static const int16_t arrMiddleQFact[] = {
    0.5  * STEP_MULT,
    0.75 * STEP_MULT,
    1    * STEP_MULT,
    1.25 * STEP_MULT
};
static const AudioGrid adjustMiddleQFact = {arrMiddleQFact, 0, 3, 0};

static const int16_t arrTrebleCFreqK[] = {
    10   * STEP_MULT,
    12.5 * STEP_MULT,
    15   * STEP_MULT,
    17.5 * STEP_MULT
};
static const AudioGrid adjustTrebleCFreqK = {arrTrebleCFreqK, 0, 3, 0};

static const int16_t subCutFreq[] = {
    0   * STEP_MULT,
    80  * STEP_MULT,
    120 * STEP_MULT,
    160 * STEP_MULT,
};
static const AudioGrid adjustSubCutFreq = {subCutFreq, 0, 3, 0};

static const int16_t arrLoudPeakFreq[] = {
    0    * STEP_MULT,
    400  * STEP_MULT,
    800  * STEP_MULT,
    2400 * STEP_MULT
};
static const AudioGrid adjustLoudPeakFreq = {arrLoudPeakFreq, 0, 3, 0};

static AudioParam *aPar;

static const AudioApi tda7719Api = {
    .init = tda7719Init,

    .setTune = tda7719SetTune,
    .setInput = tda7719SetInput,

    .setMute = tda7719SetMute,
};

static void tda7719SetInputGain(int8_t input, int8_t gain)
{
    if (input >= TDA7719_IN_CNT) {
        input = 0;
    }

    input = inCfg2[input];

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_INPUT_CONFIG | TDA7719_AUTOINC);

    i2cSend(I2C_AMP, (uint8_t)(TDA7719_INPUT_CFG2 | (gain ? TDA7719_INPUT_GAIN_3DB : 0) |
                               TDA7719_INPUT_MD2 | input));
    i2cTransmit(I2C_AMP);
}

static uint8_t tda7719GetFaderValue(int8_t in)
{
    return (in >= 0) ? in : 0x10 + (uint8_t)(-in);
}

const AudioApi *tda7719GetApi(void)
{
    return &tda7719Api;
}

void tda7719Init(AudioParam *param)
{
    aPar = param;

    aPar->grid[AUDIO_TUNE_VOLUME]      = &gridVolume;
    aPar->grid[AUDIO_TUNE_BASS]        = &gridToneBal;
    aPar->grid[AUDIO_TUNE_MIDDLE]      = &gridToneBal;
    aPar->grid[AUDIO_TUNE_TREBLE]      = &gridToneBal;
    aPar->grid[AUDIO_TUNE_PREAMP]      = &gridToneBal;
    aPar->grid[AUDIO_TUNE_LOUDNESS]    = &gridLoudness;

    aPar->grid[AUDIO_TUNE_BALANCE]     = &gridToneBal;
    aPar->grid[AUDIO_TUNE_GAIN]        = &gridGain;

    if (aPar->mode == AUDIO_MODE_4_0 ||
        aPar->mode == AUDIO_MODE_4_1) {
        aPar->grid[AUDIO_TUNE_FRONTREAR] = &gridToneBal;
    }
    if (aPar->mode == AUDIO_MODE_2_1 ||
        aPar->mode == AUDIO_MODE_4_1) {
        aPar->grid[AUDIO_TUNE_SUBWOOFER] = &gridSubwoofer;
        aPar->grid[AUDIO_TUNE_SUB_CUT_FREQ] = &adjustSubCutFreq;
    }

    uint8_t subDisable = TDA7719_SUB_DISABLE;

    if (aPar->mode == AUDIO_MODE_2_1 ||
        aPar->mode == AUDIO_MODE_4_1) {
        subDisable = 0;
    }

    aPar->grid[AUDIO_TUNE_LOUD_PEAK_FREQ]  = &adjustLoudPeakFreq;
    aPar->grid[AUDIO_TUNE_BASS_FREQ]       = &adjustBassCFreq;
    aPar->grid[AUDIO_TUNE_BASS_QUAL]       = &adjustBassQFact;
    aPar->grid[AUDIO_TUNE_MIDDLE_KFREQ]    = &adjustMiddleCFreqK;
    aPar->grid[AUDIO_TUNE_MIDDLE_QUAL]     = &adjustMiddleQFact;
    aPar->grid[AUDIO_TUNE_TREBLE_KFREQ]    = &adjustTrebleCFreqK;


    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_INPUT_CONFIG | TDA7719_AUTOINC);

    i2cSend(I2C_AMP, TDA7719_INPUT_CFG2 | TDA7719_INPUT_GAIN_3DB | TDA7719_INPUT_MD2 | 0);
    i2cSend(I2C_AMP, TDA7719_QD4_BYPASS_SUB | TDA7719_QD3_BYPASS_REAR | TDA7719_QD2_BYPASS_FRONT |
            TDA7719_2ND_IN_GAIN_3DB | TDA7719_2ND_IN_MD2 | 0);
    i2cSend(I2C_AMP, 0xFF);
    i2cSend(I2C_AMP, TDA7719_LM_RESET | TDA7719_REF_OUT_EXT | TDA7719_REAR_MAIN_IN | 0x0F);
    i2cSend(I2C_AMP, TDA7719_BYPASS_ANTI_ALIAS | TDA7719_FAST_CHARGE_OFF | subDisable |
            TDA7719_MUTE_IIC_ONLY | TDA7719_SM_OFF);
    i2cSend(I2C_AMP, 0xFF);
    i2cSend(I2C_AMP, TDA7719_SPIKE_REJ_TIME_MASK | 0x07);
    i2cSend(I2C_AMP, TDA7719_HIGH_BOOST_OFF | TDA7719_LOUD_FREQ_800HZ);
    i2cSend(I2C_AMP, 0xFF);
    i2cSend(I2C_AMP, TDA7719_TREBLE_FREQ_10K0 | TDA7719_TREBLE_GAIN_MASK);
    i2cSend(I2C_AMP, TDA7719_MIDDLE_QFACT_0P5 | TDA7719_MIDDLE_GAIN_MASK);
    i2cSend(I2C_AMP, TDA7719_BASS_QFACT_1P0 | TDA7719_BASS_GAIN_MASK);
    i2cSend(I2C_AMP, TDA7719_BASS_DCMODE_OFF | TDA7719_BASS_FREQ_60HZ | TDA7719_MIDDLE_FREQ_1000HZ |
            TDA7719_SUB_PHASE_0 | TDA7719_SUB_CUT_FREQ_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);

    i2cTransmit(I2C_AMP);
}

static void tda7719SetTrebleFilter(void)
{
    int8_t value = aPar->tune[AUDIO_TUNE_TREBLE];

    uint8_t reg09 = (value > 0) ? (31 - value) : (15 + value);

    reg09 <<= TDA7719_MIDDLE_ATT_OFT;
    reg09 |= (aPar->tune[AUDIO_TUNE_TREBLE_KFREQ] < TDA7719_TREBLE_FREQ_OFT);

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_TREBLE);
    i2cSend(I2C_AMP, reg09);
    i2cTransmit(I2C_AMP);
}

static void tda7719SetMiddleFilter(void)
{
    int8_t value = aPar->tune[AUDIO_TUNE_MIDDLE];
    uint8_t reg10 = (value > 0) ? (31 - value) : (15 + value);

    reg10 <<= TDA7719_MIDDLE_ATT_OFT;
    reg10 |= (aPar->tune[AUDIO_TUNE_MIDDLE_QUAL] < TDA7719_MIDDLE_QFACT_OFT);

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_MIDDLE);
    i2cSend(I2C_AMP, reg10);
    i2cTransmit(I2C_AMP);
}

static void tda7719SetBassFilter(void)
{
    int8_t value = aPar->tune[AUDIO_TUNE_BASS];
    uint8_t reg11 = (value > 0) ? (31 - value) : (15 + value);

    reg11 <<= TDA7719_BASS_ATT_OFT;
    reg11 |= (aPar->tune[AUDIO_TUNE_BASS_QUAL] << TDA7719_BASS_QFACT_OFT);

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_BASS);
    i2cSend(I2C_AMP, reg11);
    i2cTransmit(I2C_AMP);
}

static void tda7719SetSubwooferMiddleBass(void)
{
    uint8_t reg12 = TDA7719_BASS_DCMODE_OFF | TDA7719_SUB_PHASE_0;

    reg12 |= (aPar->tune[AUDIO_TUNE_BASS_FREQ] << TDA7719_BASS_FREQ_OFT);
    reg12 |= (aPar->tune[AUDIO_TUNE_MIDDLE_KFREQ] << TDA7719_MIDDLE_FREQ_OFT);
    reg12 |= (aPar->tune[AUDIO_TUNE_SUB_CUT_FREQ] << TDA7719_SUB_CUT_FREQ_OFT);

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_SUB_MID_BASS);
    i2cSend(I2C_AMP, reg12);
    i2cTransmit(I2C_AMP);
}

static void tda7719SetLoudness()
{
    uint8_t reg07 = TDA7719_HIGH_BOOST_OFF;

    reg07 |= ((-aPar->tune[AUDIO_TUNE_LOUDNESS]) << TDA7719_LOUDNESS_ATT_OFT);
    reg07 |= (aPar->tune[AUDIO_TUNE_LOUD_PEAK_FREQ] << TDA7719_LOUD_FREQ_OFT);

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_LOUDNESS);
    i2cSend(I2C_AMP, reg07);
    i2cTransmit(I2C_AMP);
}

void tda7719SetTune(AudioTune tune, int8_t value)
{
    (void)value;

    AudioRaw raw;
    audioSetRawBalance(&raw, aPar->tune[AUDIO_TUNE_VOLUME], false);

    // Force direct signal for spectrum analyzer in lower modes
    if (aPar->mode == AUDIO_MODE_2_0 ||
        aPar->mode == AUDIO_MODE_4_0) {
        raw.subwoofer = 0;
    }
    if (aPar->mode == AUDIO_MODE_2_0 ||
        aPar->mode == AUDIO_MODE_2_1) {
        raw.rearLeft = 0;
        raw.rearRight = 0;
    }

    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_BALANCE:
    case AUDIO_TUNE_FRONTREAR:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7719_SP_LEFT_FRONT | TDA7719_AUTOINC);
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.frontLeft));
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.frontRight));
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.rearLeft));
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.rearRight));
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.subwoofer));
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.subwoofer));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BASS:
    case AUDIO_TUNE_BASS_QUAL:
        tda7719SetBassFilter();
        break;
    case AUDIO_TUNE_MIDDLE:
    case AUDIO_TUNE_MIDDLE_QUAL:
        tda7719SetMiddleFilter();
        break;
    case AUDIO_TUNE_TREBLE:
    case AUDIO_TUNE_TREBLE_KFREQ:
        tda7719SetTrebleFilter();
        break;
    case AUDIO_TUNE_SUBWOOFER:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7719_SUB_LEFT | TDA7719_AUTOINC);
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.subwoofer));
        i2cSend(I2C_AMP, tda7719GetFaderValue(raw.subwoofer));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_PREAMP:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)TDA7719_VOLUME_OUTGAIN);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | TDA7719_OUT_GAIN_0DB));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_LOUDNESS:
    case AUDIO_TUNE_LOUD_PEAK_FREQ:
        tda7719SetLoudness();
        break;
    case AUDIO_TUNE_BASS_FREQ:
    case AUDIO_TUNE_MIDDLE_KFREQ:
    case AUDIO_TUNE_SUB_CUT_FREQ:
        tda7719SetSubwooferMiddleBass();
        break;

    case AUDIO_TUNE_GAIN:
        tda7719SetInputGain(aPar->input, value);
        break;
    default:
        break;
    }
}

void tda7719SetInput(int8_t value)
{
    tda7719SetInputGain(value, aPar->tune[AUDIO_TUNE_GAIN]);
}

void tda7719SetMute(bool value)
{
    if (value) {
        // Mute front always
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7719_SP_LEFT_FRONT | TDA7719_AUTOINC);
        i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
        i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
        i2cTransmit(I2C_AMP);
        if (aPar->mode == AUDIO_MODE_4_0 ||
            aPar->mode == AUDIO_MODE_4_1) {
            i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
            i2cSend(I2C_AMP, TDA7719_SP_LEFT_REAR | TDA7719_AUTOINC);
            i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
            i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
            i2cTransmit(I2C_AMP);
        }
        if (aPar->mode == AUDIO_MODE_2_1 ||
            aPar->mode == AUDIO_MODE_4_1) {
            i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
            i2cSend(I2C_AMP, TDA7719_SUB_LEFT | TDA7719_AUTOINC);
            i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
            i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
            i2cTransmit(I2C_AMP);
        }
    } else {
        tda7719SetTune(AUDIO_TUNE_VOLUME, aPar->tune[AUDIO_TUNE_VOLUME]);
    }
}

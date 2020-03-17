#include "tda7719.h"

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
#define TDA7719_TREBLE_FREQ_10K0    0x00
#define TDA7719_TREBLE_FREQ_12K5    0x20
#define TDA7719_TREBLE_FREQ_15K5    0x40
#define TDA7719_TREBLE_FREQ_17K5    0x60
#define TDA7719_TREBLE_GAIN_MASK    0x1F

// 10: Middle filter
#define TDA7719_MIDDLE_QFACT_0P5    0x00
#define TDA7719_MIDDLE_QFACT_0P75   0x20
#define TDA7719_MIDDLE_QFACT_1P0    0x40
#define TDA7719_MIDDLE_QFACT_1P25   0x60
#define TDA7719_MIDDLE_GAIN_MASK    0x1F

// 11: Bass filter
#define TDA7719_BASS_QFACT_1P0      0x00
#define TDA7719_BASS_QFACT_1P25     0x20
#define TDA7719_BASS_QFACT_1P5      0x60
#define TDA7719_BASS_QFACT_2P0      0x40

#define TDA7719_BASS_GAIN_MASK      0x1F

// 12: Subwoofer / middle / bass
#define TDA7719_BASS_DC_MODE_OFF    0x80
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

static const AudioGrid gridVolume  = {-79,  0, (int8_t)(1.00 * STEP_MULT)}; // -79..0dB with 1dB step
static const AudioGrid gridToneBal = {-15, 15, (int8_t)(1.00 * STEP_MULT)}; // -15..15dB with 1dB step
static const AudioGrid gridGain    = {  0,  1, (int8_t)(3.00 * STEP_MULT)}; // 0..3dB with 3dB step

static AudioParam *aPar;

static const AudioApi tda7719Api = {
    .initParam = tda7719InitParam,
    .reset = tda7719Reset,

    .setTune = tda7719SetTune,
    .setInput = tda7719SetInput,

    .setMute = tda7719SetMute,
    .setLoudness = tda7719SetLoudness,
};

static void tda7719SetInputGain(int8_t input, int8_t gain)
{
    if (input >= TDA7719_IN_CNT) {
        input = 0;
    }

    input = inCfg2[input];

    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_INPUT_CONFIG | TDA7719_AUTOINC);

    i2cSend(I2C_AMP, (uint8_t)(TDA7719_INPUT_CFG2 | (gain ? TDA7719_INPUT_GAIN_3DB : 0) | TDA7719_INPUT_MD2 | input));
    i2cTransmit(I2C_AMP);
}

const AudioApi *tda7719GetApi(void)
{
    return &tda7719Api;
}

void tda7719InitParam(AudioParam *param)
{
    aPar = param;

    aPar->tune[AUDIO_TUNE_VOLUME].grid      = &gridVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid        = &gridToneBal;
    aPar->tune[AUDIO_TUNE_MIDDLE].grid      = &gridToneBal;
    aPar->tune[AUDIO_TUNE_TREBLE].grid      = &gridToneBal;
    aPar->tune[AUDIO_TUNE_PREAMP].grid      = &gridToneBal;
    aPar->tune[AUDIO_TUNE_BALANCE].grid     = &gridToneBal;
    aPar->tune[AUDIO_TUNE_FRONTREAR].grid   = &gridToneBal;
    aPar->tune[AUDIO_TUNE_GAIN].grid        = &gridGain;
}

void tda7719Reset(void)
{
    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_INPUT_CONFIG | TDA7719_AUTOINC);

    i2cSend(I2C_AMP, TDA7719_INPUT_CFG2 | TDA7719_INPUT_GAIN_3DB | TDA7719_INPUT_MD2 | 0);
    i2cSend(I2C_AMP, TDA7719_QD4_BYPASS_SUB | TDA7719_QD3_BYPASS_REAR | TDA7719_QD2_BYPASS_FRONT | TDA7719_2ND_IN_GAIN_3DB | TDA7719_2ND_IN_MD2 | 0);
    i2cSend(I2C_AMP, 0xFF);
    i2cSend(I2C_AMP, TDA7719_LM_RESET | TDA7719_REF_OUT_EXT | TDA7719_REAR_MAIN_IN | 0x0F);
    i2cSend(I2C_AMP, TDA7719_BYPASS_ANTI_ALIAS | TDA7719_FAST_CHARGE_OFF | TDA7719_SUB_DISABLE | TDA7719_MUTE_IIC_ONLY | TDA7719_SM_OFF);
    i2cSend(I2C_AMP, 0xFF);
    i2cSend(I2C_AMP, TDA7719_SPIKE_REJ_TIME_MASK | 0x07);
    i2cSend(I2C_AMP, TDA7719_HIGH_BOOST_OFF | TDA7719_LOUD_FREQ_800HZ);
    i2cSend(I2C_AMP, 0xFF);
    i2cSend(I2C_AMP, TDA7719_TREBLE_FREQ_10K0 | TDA7719_TREBLE_GAIN_MASK);
    i2cSend(I2C_AMP, TDA7719_MIDDLE_QFACT_0P5 | TDA7719_MIDDLE_GAIN_MASK);
    i2cSend(I2C_AMP, TDA7719_BASS_QFACT_1P0 | TDA7719_BASS_GAIN_MASK);
    i2cSend(I2C_AMP, TDA7719_BASS_DC_MODE_OFF | TDA7719_BASS_FREQ_60HZ | TDA7719_MIDDLE_FREQ_1000HZ | TDA7719_SUB_PHASE_0 | TDA7719_SUB_CUT_FREQ_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
    i2cSend(I2C_AMP, 0x10);
    i2cSend(I2C_AMP, 0x10);

    i2cTransmit(I2C_AMP);
}

void tda7719SetTune(AudioTune tune, int8_t value)
{
    (void)value;

    int8_t spFrontLeft = aPar->tune[AUDIO_TUNE_VOLUME].value;
    int8_t spFrontRight = aPar->tune[AUDIO_TUNE_VOLUME].value;
    int8_t spRearLeft = aPar->tune[AUDIO_TUNE_VOLUME].value;
    int8_t spRearRight = aPar->tune[AUDIO_TUNE_VOLUME].value;

    if (aPar->tune[AUDIO_TUNE_BALANCE].value > 0) {
        spFrontLeft -= aPar->tune[AUDIO_TUNE_BALANCE].value;
        spRearLeft -= aPar->tune[AUDIO_TUNE_BALANCE].value;
    } else {
        spFrontRight += aPar->tune[AUDIO_TUNE_BALANCE].value;
        spRearRight += aPar->tune[AUDIO_TUNE_BALANCE].value;
    }
    if (aPar->tune[AUDIO_TUNE_FRONTREAR].value > 0) {
        spRearLeft -= aPar->tune[AUDIO_TUNE_FRONTREAR].value;
        spRearRight -= aPar->tune[AUDIO_TUNE_FRONTREAR].value;
    } else {
        spFrontLeft += aPar->tune[AUDIO_TUNE_FRONTREAR].value;
        spFrontRight += aPar->tune[AUDIO_TUNE_FRONTREAR].value;
    }

    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_BALANCE:
    case AUDIO_TUNE_FRONTREAR:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7719_SP_LEFT_FRONT | TDA7719_AUTOINC);
        i2cSend(I2C_AMP, 0x10 + (uint8_t)(-spFrontLeft));
        i2cSend(I2C_AMP, 0x10 + (uint8_t)(-spFrontRight));
        i2cSend(I2C_AMP, 0x10 + (uint8_t)(-spRearLeft));
        i2cSend(I2C_AMP, 0x10 + (uint8_t)(-spRearRight));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BASS:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)TDA7719_BASS);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | TDA7719_BASS_QFACT_1P0));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_MIDDLE:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)TDA7719_MIDDLE);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | TDA7719_MIDDLE_QFACT_0P5));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_TREBLE:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)TDA7719_TREBLE);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | TDA7719_TREBLE_FREQ_10K0));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_PREAMP:
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)TDA7719_VOLUME_OUTGAIN);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | TDA7719_OUT_GAIN_0DB));
        i2cTransmit(I2C_AMP);
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
    tda7719SetInputGain(value, aPar->tune[AUDIO_TUNE_GAIN].value);
}

void tda7719SetMute(bool value)
{
    if (value) {
        i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7719_SP_LEFT_FRONT | TDA7719_AUTOINC);
        i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
        i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
        i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
        i2cSend(I2C_AMP, TDA7719_SP_ATT_MASK);
        i2cTransmit(I2C_AMP);
    } else {
        tda7719SetTune(AUDIO_TUNE_VOLUME, aPar->tune[AUDIO_TUNE_VOLUME].value);
    }
}

void tda7719SetLoudness(bool value)
{
    i2cBegin(I2C_AMP, TDA7719_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7719_LOUDNESS);
    i2cSend(I2C_AMP, TDA7719_HIGH_BOOST_OFF | TDA7719_LOUD_FREQ_800HZ | (value ? 7 : 0));
    i2cTransmit(I2C_AMP);
}

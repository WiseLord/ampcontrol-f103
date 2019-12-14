#include "tda7719.h"

#include "../i2c.h"
#include "../pins.h"

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
#define TDA7719_LOUD_FREQ_MASK      0x30
#define TDA7719_LOUD_ATT_MASK       0x0F

// 8: Volume / output gain
#define TDA7719_OUT_GAIN_0DB        0x40
#define TDA7719_GAIN_MASK           0x1F

// 9: Treble filter
#define TDA7719_TREBLE_FREQ_MASK    0x60
#define TDA7719_TREBLE_GAIN_MASK    0x1F

// 10: Middle filter
#define TDA7719_MIDDLE_QFACT_MASK   0x60
#define TDA7719_MIDDLE_GAIN_MASK    0x1F

// 11: Bass filter
#define TDA7719_BASS_QFACT_MASK     0x60
#define TDA7719_BASS_GAIN_MASK      0x1F

// 12: Subwoofer / middle / bass
#define TDA7719_BASS_FREQ_MASK      0x60
#define TDA7719_MIDDLE_FREQ_MASK    0x18
#define TDA7719_SUB_PHASE           0x04
#define TDA7719_SUB_CUT_FREQ_MASK   0x03

// 13..16: Speaker attenuation
#define TDA7719_SP_ATT_MASK         0x7F

// 17..18: Subwoofer L/R attenuation
#define TDA7719_SUB_ATT_MASK        0x7F

// Common masks
#define TDA7719_INPUT_MASK          0x07
#define TDA7719_SOFT_STEP           0x80

#define TDA7719_INPUT_CFG           TDA7719_INPUT_CFG2


//static const uint8_t inCfg2[TDA7719_IN_CNT] = { 0, 1, 2, 4, 5, 6 };

static const AudioGrid gridVolume  = {-79,  0, (uint8_t)(1.00 * 8)}; // -79..0dB with 1dB step
static const AudioGrid gridToneBal = {-15, 15, (uint8_t)(1.00 * 8)}; // -15..15dB with 1dB step
static const AudioGrid gridGain    = {  0,  1, (uint8_t)(3.00 * 8)}; // 0..3dB with 3dB step

static AudioParam *aPar;

static const AudioApi tda7719Api = {
    .init = tda7719Init,

    .setTune = tda7719SetTune,
    .setInput = tda7719SetInput,

    .setMute = tda7719SetMute,
};

const AudioApi *tda7719GetApi(void)
{
    return &tda7719Api;
}

void tda7719Init(AudioParam *param)
{
    aPar = param;

    aPar->tune[AUDIO_TUNE_VOLUME].grid  = &gridVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid    = &gridToneBal;
    aPar->tune[AUDIO_TUNE_MIDDLE].grid  = &gridToneBal;
    aPar->tune[AUDIO_TUNE_TREBLE].grid  = &gridToneBal;
    aPar->tune[AUDIO_TUNE_BALANCE].grid = &gridToneBal;
    aPar->tune[AUDIO_TUNE_GAIN].grid    = &gridGain;
}

void tda7719SetTune(AudioTune tune, int8_t value)
{
    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_BALANCE:
        break;
    case AUDIO_TUNE_BASS:
    case AUDIO_TUNE_MIDDLE:
    case AUDIO_TUNE_TREBLE:
        break;
    case AUDIO_TUNE_GAIN:
        break;
    default:
        break;
    }
}

void tda7719SetInput(uint8_t value)
{
    (void)value;
}

void tda7719SetMute(bool value)
{
    (void)value;
}

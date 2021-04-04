#include "tda7418.h"

#include "audio.h"
#include "hwlibs.h"
#include "i2c.h"

// I2C address
#define TDA7418_I2C_ADDR            0x88

// I2C function (subaddress) selection
#define TDA7418_SOURCE_SELECT       0x00
#define TDA7418_LOUDNESS            0x01
#define TDA7418_VOLUME              0x02
#define TDA7418_TREBLE              0x03
#define TDA7418_MIDDLE              0x04
#define TDA7418_BASS                0x05
#define TDA7418_MID_BASS_FC_SELECT  0x06
#define TDA7418_SP_FRONT_LEFT       0x07
#define TDA7418_SP_REAR_LEFT        0x08
#define TDA7418_SP_REAR_RIGHT       0x09
#define TDA7418_SP_FRONT_RIGHT      0x0A
#define TDA7418_SUBWOOFER           0x0B
#define TDA7418_SOFTMUTE            0x0C
#define TDA7418_TESTING_AUDIO       0x0D

// Subaddress
#define TDA7418_TESTING_MODE        0x80
#define TDA7418_AUTO_ZERO_REMAIN    0x40
#define TDA7418_AUTO_INC            0x20

// 0: Input selector / gain bits
#define TDA7418_DIFFIN_MODE         0x80

// 1: Loudness
#define TDA7418_LOUDNESS_ATT_OFT    0
#define TDA7418_LOUD_FREQ_OFT       4

#define TDA7448_LOUD_ATTN_MASK      0x0F
#define TDA7448_LOUD_ATTN_MASK      0x0F
#define TDA7448_LOUD_FREQ_OFF       0x00
#define TDA7448_LOUD_FREQ_400HZ     0x10
#define TDA7448_LOUD_FREQ_800HZ     0x20
#define TDA7448_LOUD_FREQ_2400HZ    0x30
#define TDA7448_LOUD_SHAPE_LO       0x00
#define TDA7448_LOUD_SHAPE_HILO     0x40
#define TDA7448_LOUD_SOFTSTEP_ON    0x00
#define TDA7448_LOUD_SOFTSTEP_OFF   0x80

// 3: Treble
#define TDA7418_TREBLE_ATT_OFT      0
#define TDA7418_TREBLE_FREQ_OFT     5

#define TDA7418_TREBLE_CENTER_10K0  0x80
#define TDA7418_TREBLE_CENTER_12K5  0xA0
#define TDA7418_TREBLE_CENTER_15K0  0xC0
#define TDA7418_TREBLE_CENTER_17K5  0xE0

// 4: Middle
#define TDA7418_MIDDLE_ATT_OFT      0
#define TDA7418_MIDDLE_QFACT_OFT    5

#define TDA7418_MIDDLE_Q_0P50       0x00
#define TDA7418_MIDDLE_Q_0P75       0x20
#define TDA7418_MIDDLE_Q_1P10       0x40
#define TDA7418_MIDDLE_Q_1P25       0x60
#define TDA7418_MIDDLE_SOFTSTEP_OFF 0x80

// 5: Bass
#define TDA7418_BASS_ATT_OFT        0
#define TDA7418_BASS_QFACT_OFT      5

#define TDA7418_BASS_Q_1P00         0x00
#define TDA7418_BASS_Q_1P25         0x20
#define TDA7418_BASS_Q_1P50         0x40
#define TDA7418_BASS_Q_2P00         0x60
#define TDA7418_BASS_SOFTSTEP_OFF   0x80

// 6: Middle/bass FC select
#define TDA7418_MIDDLE_FREQ_OFT     0
#define TDA7418_BASS_FREQ_OFT       2

#define TDA7418_MIDDLE_CENTER_0500  0x00
#define TDA7418_MIDDLE_CENTER_1000  0x01
#define TDA7418_MIDDLE_CENTER_1500  0x02
#define TDA7418_MIDDLE_CENTER_2500  0x03
#define TDA7418_BASS_CENTER_060     0x00
#define TDA7418_BASS_CENTER_080     0x04
#define TDA7418_BASS_CENTER_100     0x08
#define TDA7418_BASS_CENTER_200     0x0C
#define TDA7418_BASS_DC_MODE        0x10
#define TDA7418_BASS_SMOOTH_FILTER  0x20

// Softmute
#define TDA7418_SOFTMUTE_ON         0x00
#define TDA7418_SOFTMUTE_OFF        0x01
#define TDA7418_SOFTMUTE_OP48MS     0x00
#define TDA7418_SOFTMUTE_OP96MS     0x02
#define TDA7418_SOFTMUTE_1P23MS     0x04

static const AudioGrid gridVolume    = {NULL, -79, 15, (int8_t)(1.00 * STEP_MULT)}; // -79..15dB with 1dB step
static const AudioGrid gridToneBal   = {NULL, -15, 15, (int8_t)(1.00 * STEP_MULT)}; // -15..15dB with 1dB step
static const AudioGrid gridSubwoofer = {NULL, -15,  0, (int8_t)(1.00 * STEP_MULT)}; // -15..0dB with 1dB step
static const AudioGrid gridGain      = {NULL,   0, 15, (int8_t)(1.00 * STEP_MULT)}; // 0..15dB with 1dB step
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

static const int16_t arrLoudPeakFreq[] = {
    0    * STEP_MULT,
    400  * STEP_MULT,
    800  * STEP_MULT,
    2400 * STEP_MULT
};
static const AudioGrid adjustLoudPeakFreq = {arrLoudPeakFreq, 0, 3, 0};

static AudioParam *aPar;

static const AudioApi tda7418Api = {
    .init = tda7418Init,

    .setTune = tda7418SetTune,
    .setInput = tda7418SetInput,

    .setMute = tda7418SetMute,
};

static void tda7418InputGain(int8_t input, int8_t gain)
{
    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_SOURCE_SELECT);
    i2cSend(I2C_AMP, (uint8_t) (gain << 3) | (input & 0x03) | TDA7418_DIFFIN_MODE );
    i2cTransmit(I2C_AMP);
}

static void tda7418SetSpeakers(void)
{
    AudioRaw raw;
    audioSetRawBalance(&raw, 0, false);

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_SP_FRONT_LEFT | TDA7418_AUTO_INC);
    i2cSend(I2C_AMP, (uint8_t)(16 - raw.frontLeft));
    i2cSend(I2C_AMP, (uint8_t)(16 - raw.rearLeft));
    i2cSend(I2C_AMP, (uint8_t)(16 - raw.rearRight));
    i2cSend(I2C_AMP, (uint8_t)(16 - raw.frontRight));
    i2cTransmit(I2C_AMP);
}

const AudioApi *tda7418GetApi(void)
{
    return &tda7418Api;
}

void tda7418Init(AudioParam *param)
{
    aPar = param;

    aPar->grid[AUDIO_TUNE_VOLUME]    = &gridVolume;
    aPar->grid[AUDIO_TUNE_BASS]      = &gridToneBal;
    aPar->grid[AUDIO_TUNE_MIDDLE]    = &gridToneBal;
    aPar->grid[AUDIO_TUNE_TREBLE]    = &gridToneBal;
    aPar->grid[AUDIO_TUNE_LOUDNESS]  = &gridLoudness;

    if (aPar->mode == AUDIO_MODE_4_0 ||
        aPar->mode == AUDIO_MODE_4_1) {
        aPar->grid[AUDIO_TUNE_FRONTREAR] = &gridToneBal;
    }
    aPar->grid[AUDIO_TUNE_BALANCE]   = &gridToneBal;
    if (aPar->mode == AUDIO_MODE_2_1 ||
        aPar->mode == AUDIO_MODE_4_1) {
        aPar->grid[AUDIO_TUNE_SUBWOOFER] = &gridSubwoofer;
    }
    aPar->grid[AUDIO_TUNE_GAIN]      = &gridGain;

    aPar->grid[AUDIO_TUNE_LOUD_PEAK_FREQ]  = &adjustLoudPeakFreq;
    aPar->grid[AUDIO_TUNE_BASS_FREQ]       = &adjustBassCFreq;
    aPar->grid[AUDIO_TUNE_BASS_QUAL]       = &adjustBassQFact;
    aPar->grid[AUDIO_TUNE_MIDDLE_KFREQ]    = &adjustMiddleCFreqK;
    aPar->grid[AUDIO_TUNE_MIDDLE_QUAL]     = &adjustMiddleQFact;
    aPar->grid[AUDIO_TUNE_TREBLE_KFREQ]    = &adjustTrebleCFreqK;
}

static void tda7418SetTrebleFilter(void)
{
    int8_t value = (aPar->flags & AUDIO_FLAG_BYPASS) ? 0 : aPar->tune[AUDIO_TUNE_TREBLE];

    uint8_t reg03 = (value > 0) ? (31 - value) : (15 + value);

    reg03 <<= TDA7418_MIDDLE_ATT_OFT;
    reg03 |= (aPar->tune[AUDIO_TUNE_TREBLE_KFREQ] < TDA7418_TREBLE_FREQ_OFT);
    reg03 |= 0x80;

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_TREBLE);
    i2cSend(I2C_AMP, reg03);
    i2cTransmit(I2C_AMP);
}

static void tda7418SetMiddleFilter(void)
{
    int8_t value = (aPar->flags & AUDIO_FLAG_BYPASS) ? 0 : aPar->tune[AUDIO_TUNE_MIDDLE];

    uint8_t reg04 = (value > 0) ? (31 - value) : (15 + value);

    reg04 <<= TDA7418_MIDDLE_ATT_OFT;
    reg04 |= (aPar->tune[AUDIO_TUNE_MIDDLE_QUAL] < TDA7418_MIDDLE_QFACT_OFT);

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_MIDDLE);
    i2cSend(I2C_AMP, reg04);
    i2cTransmit(I2C_AMP);
}

static void tda7418SetBassFilter(void)
{
    int8_t value = (aPar->flags & AUDIO_FLAG_BYPASS) ? 0 : aPar->tune[AUDIO_TUNE_BASS];

    uint8_t reg05 = (value > 0) ? (31 - value) : (15 + value);

    reg05 <<= TDA7418_BASS_ATT_OFT;
    reg05 |= (aPar->tune[AUDIO_TUNE_BASS_QUAL] << TDA7418_BASS_QFACT_OFT);

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_BASS);
    i2cSend(I2C_AMP, reg05);
    i2cTransmit(I2C_AMP);
}

static void tda7418SetMiddleBassFcFilter(void)
{
    uint8_t reg06 = 0;

    reg06 |= (aPar->tune[AUDIO_TUNE_BASS_FREQ] << TDA7418_BASS_FREQ_OFT);
    reg06 |= (aPar->tune[AUDIO_TUNE_MIDDLE_KFREQ] << TDA7418_MIDDLE_FREQ_OFT);

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_MID_BASS_FC_SELECT);
    i2cSend(I2C_AMP, reg06);
    i2cTransmit(I2C_AMP);
}

static void tda7418SetLoudness(void)
{
    uint8_t reg01 = 0;

    reg01 |= ((-aPar->tune[AUDIO_TUNE_LOUDNESS]) << TDA7418_LOUDNESS_ATT_OFT);
    reg01 |= (aPar->tune[AUDIO_TUNE_LOUD_PEAK_FREQ] << TDA7418_LOUD_FREQ_OFT);

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_LOUDNESS);
    i2cSend(I2C_AMP, reg01);
    i2cTransmit(I2C_AMP);
}

void tda7418SetTune(AudioTune tune, int8_t value)
{
    switch (tune) {
    case AUDIO_TUNE_VOLUME:
        i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7418_VOLUME);
        i2cSend(I2C_AMP, (uint8_t)(value > 0 ? value : 16 - value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BASS:
    case AUDIO_TUNE_BASS_QUAL:
        tda7418SetBassFilter();
        break;
    case AUDIO_TUNE_MIDDLE:
    case AUDIO_TUNE_MIDDLE_QUAL:
        tda7418SetMiddleFilter();
        break;
    case AUDIO_TUNE_TREBLE:
    case AUDIO_TUNE_TREBLE_KFREQ:
        tda7418SetTrebleFilter();
        break;
    case AUDIO_TUNE_FRONTREAR:
        tda7418SetSpeakers();
        break;
    case AUDIO_TUNE_BALANCE:
        tda7418SetSpeakers();
        break;
    case AUDIO_TUNE_SUBWOOFER:
        i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7418_SUBWOOFER);
        i2cSend(I2C_AMP, (uint8_t)(value > 0 ? value : 16 - value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_LOUDNESS:
    case AUDIO_TUNE_LOUD_PEAK_FREQ:
        tda7418SetLoudness();
        break;
    case AUDIO_TUNE_BASS_FREQ:
    case AUDIO_TUNE_MIDDLE_KFREQ:
        tda7418SetMiddleBassFcFilter();
        break;
    case AUDIO_TUNE_GAIN:
        tda7418InputGain(aPar->input, value);
        break;
    default:
        break;
    }
}

void tda7418SetInput(int8_t value)
{
    tda7418InputGain(value, aPar->tune[AUDIO_TUNE_GAIN]);
}

void tda7418SetMute(bool value)
{
    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_SOFTMUTE);
    i2cSend(I2C_AMP, (uint8_t)(value ? TDA7418_SOFTMUTE_ON : TDA7418_SOFTMUTE_OFF));
    i2cTransmit(I2C_AMP);
}

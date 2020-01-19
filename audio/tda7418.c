#include "tda7418.h"

#include "../i2c.h"
#include "../pins.h"

// I2C address
#define TDA7418_I2C_ADDR            0x88

// I2C function (subaddress) selection
#define TDA7418_SOURCE_SELECT       0x00
#define TDA7418_LOUDNESS            0x01
#define TDA7418_VOLUME              0x02
#define TDA7439_TREBLE              0x03
#define TDA7439_MIDDLE              0x04
#define TDA7439_BASS                0x05
#define TDA7439_MID_BASS_FC_SELECT  0x06
#define TDA7418_SP_FRONT_LEFT       0x07
#define TDA7418_SP_REAR_LEFT        0x08
#define TDA7418_SP_REAR_RIGHT       0x09
#define TDA7418_SP_FRONT_RIGHT      0x0A
#define TDA7418_SUBWOOFER           0x0B
#define TDA7418_SOFTMUTE            0x0C
#define TDA7418_TESTING_AUDIO       0x0D

// Subaddress bits
#define TDA7418_TESTING_MODE        0x80
#define TDA7418_AUTO_ZERO_REMAIN    0x40
#define TDA7418_AUTO_INC            0x20

// Input selector / gain bits
#define TDA7418_DIFFIN_MODE         0x80

// Loudness bits
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

// Treble bits
#define TDA7418_TREBLE_CENTER_10K0  0x80
#define TDA7418_TREBLE_CENTER_12K5  0x82
#define TDA7418_TREBLE_CENTER_15K0  0x84
#define TDA7418_TREBLE_CENTER_17K5  0x86

// Middle bits
#define TDA7418_MIDDLE_Q_0P50       0x00
#define TDA7418_MIDDLE_Q_0P75       0x02
#define TDA7418_MIDDLE_Q_1P10       0x04
#define TDA7418_MIDDLE_Q_1P25       0x06
#define TDA7418_MIDDLE_SOFTSTEP_OFF 0x80

// Bass bits
#define TDA7418_BASS_Q_1P00         0x00
#define TDA7418_BASS_Q_1P25         0x02
#define TDA7418_BASS_Q_1P50         0x04
#define TDA7418_BASS_Q_2P00         0x06
#define TDA7418_BASS_SOFTSTEP_OFF   0x80

// Middle/bass FC select
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

// Softmute bits
#define TDA7418_SOFTMUTE_ON         0x00
#define TDA7418_SOFTMUTE_OFF        0x01
#define TDA7418_SOFTMUTE_OP48MS     0x00
#define TDA7418_SOFTMUTE_OP96MS     0x02
#define TDA7418_SOFTMUTE_1P23MS     0x04

static const AudioGrid gridVolume    = {-79, 15, (uint8_t)(1.00 * 8)}; // -79..15dB with 1dB step
static const AudioGrid gridToneBal   = {-15, 15, (uint8_t)(1.00 * 8)}; // -15..15dB with 1dB step
static const AudioGrid gridSubwoofer = {-15,  0, (uint8_t)(1.00 * 8)}; // -15..0dB with 1dB step
static const AudioGrid gridGain      = {  0, 15, (uint8_t)(1.00 * 8)}; // 0..15dB with 1dB step

static AudioParam *aPar;

static const AudioApi tda7418Api = {
    .initParam = tda7418InitParam,

    .setTune = tda7418SetTune,
    .setInput = tda7418SetInput,

    .setMute = tda7418SetMute,
    .setLoudness = tda7418SetLoudness,
};

// TODO: make this adjustable
static const uint8_t bassQ = TDA7418_BASS_Q_2P00;
static const uint8_t middleQ = TDA7418_MIDDLE_Q_1P25;
static const uint8_t trebleCF = TDA7418_TREBLE_CENTER_12K5;

static void tda7418InputGain(int8_t input, int8_t gain)
{
    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_SOURCE_SELECT);
    i2cSend(I2C_AMP, (uint8_t) (gain << 3) | (input & 0x03) | TDA7418_DIFFIN_MODE );
    i2cTransmit(I2C_AMP);
}

static void tda7418SetSpeakers(void)
{
    int8_t spFrontLeft = 0;
    int8_t spFrontRight = 0;
    int8_t spRearLeft = 0;
    int8_t spRearRight = 0;

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

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_SP_FRONT_LEFT | TDA7418_AUTO_INC);
    i2cSend(I2C_AMP, (uint8_t)(16 - spFrontLeft));
    i2cSend(I2C_AMP, (uint8_t)(16 - spRearLeft));
    i2cSend(I2C_AMP, (uint8_t)(16 - spRearRight));
    i2cSend(I2C_AMP, (uint8_t)(16 - spFrontRight));
    i2cTransmit(I2C_AMP);
}

const AudioApi *tda7418GetApi(void)
{
    return &tda7418Api;
}

void tda7418InitParam(AudioParam *param)
{
    aPar = param;

    aPar->tune[AUDIO_TUNE_VOLUME].grid    = &gridVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid      = &gridToneBal;
    aPar->tune[AUDIO_TUNE_MIDDLE].grid    = &gridToneBal;
    aPar->tune[AUDIO_TUNE_TREBLE].grid    = &gridToneBal;
    aPar->tune[AUDIO_TUNE_FRONTREAR].grid = &gridToneBal;
    aPar->tune[AUDIO_TUNE_BALANCE].grid   = &gridToneBal;
    aPar->tune[AUDIO_TUNE_SUBWOOFER].grid = &gridSubwoofer;
    aPar->tune[AUDIO_TUNE_GAIN].grid      = &gridGain;
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
        i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_BASS);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | bassQ));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_MIDDLE:
        i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_MIDDLE);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | middleQ));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_TREBLE:
        i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_TREBLE);
        i2cSend(I2C_AMP, (uint8_t)((value > 0 ? 31 - value : 15 + value) | trebleCF));
        i2cTransmit(I2C_AMP);
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
    case AUDIO_TUNE_GAIN:
        tda7418InputGain(aPar->input, value);
        break;
    default:
        break;
    }
}

void tda7418SetInput(int8_t value)
{
    tda7418InputGain(value, aPar->tune[AUDIO_TUNE_GAIN].value);
}

void tda7418SetMute(bool value)
{
    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_SOFTMUTE);
    i2cSend(I2C_AMP, (uint8_t)(value ? TDA7418_SOFTMUTE_ON : TDA7418_SOFTMUTE_OFF));
    i2cTransmit(I2C_AMP);
}

void tda7418SetLoudness(bool value)
{
    uint8_t loudness = TDA7448_LOUD_ATTN_MASK & 10;

    if (value) {
        loudness |= TDA7448_LOUD_FREQ_800HZ | TDA7448_LOUD_SHAPE_HILO;
    }

    i2cBegin(I2C_AMP, TDA7418_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7418_LOUDNESS);
    i2cSend(I2C_AMP, loudness);
    i2cTransmit(I2C_AMP);
}

#include "tda7439.h"

#include "../i2c.h"
#include "../pins.h"

// I2C address
#define TDA7439_I2C_ADDR            0x88

// I2C function selection
#define TDA7439_INPUT_SELECT        0x00
#define TDA7439_INPUT_GAIN          0x01
#define TDA7439_PREAMP              0x02
#define TDA7439_BASS                0x03
#define TDA7439_MIDDLE              0x04
#define TDA7439_TREBLE              0x05
#define TDA7439_VOLUME_RIGHT        0x06
#define TDA7439_VOLUME_LEFT         0x07

#define TDA7439_SPEAKER_MUTE        0x7F

// I2C autoincrement flag
#define TDA7439_AUTO_INC            0x10

static const AudioGrid gridVolume  = {-79,  0, (uint8_t)(1.00 * 8)}; // -79..0dB with 1dB step
static const AudioGrid gridTone    = { -7,  7, (uint8_t)(2.00 * 8)}; // -14..14dB with 2dB step
static const AudioGrid gridBalance = {-15, 15, (uint8_t)(1.00 * 8)}; // -15..15dB with 1dB step
static const AudioGrid gridPreamp  = {-47,  0, (uint8_t)(1.00 * 8)}; // -47..0dB with 1dB step
static const AudioGrid gridGain    = {  0, 15, (uint8_t)(2.00 * 8)}; // 0..30dB with 2dB step

static AudioParam *aPar;

static const AudioApi tda7439Api = {
    .initParam = tda7439InitParam,

    .setTune = tda7439SetTune,
    .setInput = tda7439SetInput,

    .setMute = tda7439SetMute,
};

const AudioApi *tda7439GetApi(void)
{
    return &tda7439Api;
}

void tda7439InitParam(AudioParam *param)
{
    aPar = param;

    aPar->tune[AUDIO_TUNE_VOLUME].grid  = &gridVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid    = &gridTone;
    if (aPar->ic == AUDIO_IC_TDA7439) {
        aPar->tune[AUDIO_TUNE_MIDDLE].grid  = &gridTone;
    }
    aPar->tune[AUDIO_TUNE_TREBLE].grid  = &gridTone;
    aPar->tune[AUDIO_TUNE_PREAMP].grid  = &gridPreamp;
    aPar->tune[AUDIO_TUNE_BALANCE].grid = &gridBalance;
    aPar->tune[AUDIO_TUNE_GAIN].grid    = &gridGain;
}

void tda7439SetTune(AudioTune tune, int8_t value)
{
    int8_t spLeft = aPar->tune[AUDIO_TUNE_VOLUME].value;
    int8_t spRight = aPar->tune[AUDIO_TUNE_VOLUME].value;
    int8_t volMin = aPar->tune[AUDIO_TUNE_VOLUME].grid->min;

    if (aPar->tune[AUDIO_TUNE_BALANCE].value > 0) {
        spLeft -= aPar->tune[AUDIO_TUNE_BALANCE].value;
        if (spLeft < volMin)
            spLeft = volMin;
    } else {
        spRight += aPar->tune[AUDIO_TUNE_BALANCE].value;
        if (spRight < volMin)
            spRight = volMin;
    }

    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_BALANCE:
        i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_VOLUME_RIGHT | TDA7439_AUTO_INC);
        i2cSend(I2C_AMP, (uint8_t)(-spRight));
        i2cSend(I2C_AMP, (uint8_t)(-spLeft));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BASS:
    case AUDIO_TUNE_MIDDLE:
    case AUDIO_TUNE_TREBLE:
        i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)(TDA7439_BASS + (tune - AUDIO_TUNE_BASS)));
        i2cSend(I2C_AMP, (uint8_t)(value > 0 ? 15 - value : 7 + value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_PREAMP:
        i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_PREAMP);
        i2cSend(I2C_AMP, (uint8_t)(-value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_GAIN:
        i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_INPUT_GAIN);
        i2cSend(I2C_AMP, (uint8_t)value);
        i2cTransmit(I2C_AMP);
        break;
    default:
        break;
    }
}

void tda7439SetInput(int8_t value)
{
    i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
    i2cSend(I2C_AMP, TDA7439_INPUT_SELECT);
    i2cSend(I2C_AMP, TDA7439_IN_CNT - 1 - aPar->input);
    i2cTransmit(I2C_AMP);
}

void tda7439SetMute(bool value)
{
    if (value) {
        i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_VOLUME_RIGHT | TDA7439_AUTO_INC);
        i2cSend(I2C_AMP, TDA7439_SPEAKER_MUTE);
        i2cSend(I2C_AMP, TDA7439_SPEAKER_MUTE);
        i2cTransmit(I2C_AMP);
    } else {
        tda7439SetTune(AUDIO_TUNE_VOLUME, aPar->tune[AUDIO_TUNE_VOLUME].value);
    }
}

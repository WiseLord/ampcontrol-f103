#include "tda7439.h"

#include "audio.h"
#include "hwlibs.h"
#include "i2c.h"

// I2C address
#define TDA7439_I2C_ADDR            0x88

#define TDA7439_IN_CNT              4

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

static const AudioGrid gridVolume  = {NULL, -79,  0, (int8_t)(1.00 * STEP_MULT)}; // -79..0dB with 1dB step
static const AudioGrid gridTone    = {NULL,  -7,  7, (int8_t)(2.00 * STEP_MULT)}; // -14..14dB with 2dB step
static const AudioGrid gridBalance = {NULL, -15, 15, (int8_t)(1.00 * STEP_MULT)}; // -15..15dB with 1dB step
static const AudioGrid gridPreamp  = {NULL, -47,  0, (int8_t)(1.00 * STEP_MULT)}; // -47..0dB with 1dB step
static const AudioGrid gridGain    = {NULL,   0, 15, (int8_t)(2.00 * STEP_MULT)}; // 0..30dB with 2dB step

static const AudioApi tda7439Api = {
    .init = tda7439Init,
    .getInCnt = tda7439GetInCnt,

    .setTune = tda7439SetTune,
    .setInput = tda7439SetInput,

    .setMute = tda7439SetMute,
};

const AudioApi *tda7439GetApi(void)
{
    return &tda7439Api;
}

void tda7439Init(AudioParam *param)
{
    aPar = param;

    aPar->grid[AUDIO_TUNE_VOLUME]  = &gridVolume;
    aPar->grid[AUDIO_TUNE_BASS]    = &gridTone;
    if (aPar->ic == AUDIO_IC_TDA7439) {
        aPar->grid[AUDIO_TUNE_MIDDLE]  = &gridTone;
    }
    aPar->grid[AUDIO_TUNE_TREBLE]  = &gridTone;
    aPar->grid[AUDIO_TUNE_PREAMP]  = &gridPreamp;
    aPar->grid[AUDIO_TUNE_BALANCE] = &gridBalance;
    aPar->grid[AUDIO_TUNE_GAIN]    = &gridGain;
}

int8_t tda7439GetInCnt(void)
{
    return TDA7439_IN_CNT;
}

void tda7439SetTune(AudioTune tune, int8_t value)
{
    AudioRaw raw;
    audioSetRawBalance(&raw, aPar->tune[AUDIO_TUNE_VOLUME], false);

    int8_t volMin = aPar->grid[AUDIO_TUNE_VOLUME]->min;
    if (raw.frontLeft < volMin) {
        raw.frontLeft = volMin;
    }
    if (raw.frontRight < volMin) {
        raw.frontRight = volMin;
    }

    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_BALANCE:
        i2cBegin(I2C_AMP, TDA7439_I2C_ADDR);
        i2cSend(I2C_AMP, TDA7439_VOLUME_RIGHT | TDA7439_AUTO_INC);
        i2cSend(I2C_AMP, (uint8_t)(-raw.frontRight));
        i2cSend(I2C_AMP, (uint8_t)(-raw.frontLeft));
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
    i2cSend(I2C_AMP, (uint8_t)(TDA7439_IN_CNT - 1 - value));
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
        tda7439SetTune(AUDIO_TUNE_VOLUME, aPar->tune[AUDIO_TUNE_VOLUME]);
    }
}

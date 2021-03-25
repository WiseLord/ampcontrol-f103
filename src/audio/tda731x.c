#include "tda731x.h"

#include "audio.h"
#include "hwlibs.h"
#include "i2c.h"

// I2C address
#define TDA731X_I2C_ADDR            0x88

// I2C function selection
#define TDA731X_VOLUME              0x00
#define TDA731X_SP_FRONT_LEFT       0x80
#define TDA731X_SP_FRONT_RIGHT      0xA0
#define TDA731X_SP_REAR_LEFT        0xC0
#define TDA731X_SP_REAR_RIGHT       0xE0
#define TDA731X_MUTE                0x1F
#define TDA731X_SW                  0x40
#define TDA731X_BASS                0x60
#define TDA731X_TREBLE              0x70

static const AudioGrid gridVolume  = {-63,  0, (int8_t)(1.25 * STEP_MULT)}; // -78.75..0dB with 1.25dB step
static const AudioGrid gridTone    = { -7,  7, (int8_t)(2.00 * STEP_MULT)}; // -14..14dB with 2dB step
static const AudioGrid gridBalance = {-15, 15, (int8_t)(1.25 * STEP_MULT)}; // -18.75..18.75dB with 1.25dB step
static const AudioGrid gridGain    = {  0,  3, (int8_t)(3.75 * STEP_MULT)}; // 0..11.25dB with 3.75dB step
static const AudioGrid gridSub     = {-15,  0, (int8_t)(1.25 * STEP_MULT)}; // -18.75..0dB with 1.25dB step

static AudioParam *aPar;

static const AudioApi tda731xApi = {
    .init = tda731xInit,

    .setTune = tda731xSetTune,
    .setInput = tda731xSetInput,

    .setMute = tda731xSetMute,
    .setLoudness = tda731xSetLoudness,
};

static void tda731xSwitch(int8_t input, int8_t gain, bool loudness)
{
    i2cBegin(I2C_AMP, TDA731X_I2C_ADDR);
    i2cSend(I2C_AMP, (uint8_t)(TDA731X_SW | input |
                               ((3 - gain) << 3) |
                               (loudness ? (1 << 2) : 0)));
    i2cTransmit(I2C_AMP);
}

const AudioApi *tda731xGetApi(void)
{
    return &tda731xApi;
}

void tda731xInit(AudioParam *param)
{
    aPar = param;

    aPar->tune[AUDIO_TUNE_VOLUME].grid    = &gridVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid      = &gridTone;
    aPar->tune[AUDIO_TUNE_TREBLE].grid    = &gridTone;
    if (aPar->mode == AUDIO_MODE_4_0) {
        aPar->tune[AUDIO_TUNE_FRONTREAR].grid = &gridBalance;
    }
    aPar->tune[AUDIO_TUNE_BALANCE].grid   = &gridBalance;
    if (aPar->mode == AUDIO_MODE_2_1) {
        aPar->tune[AUDIO_TUNE_SUBWOOFER].grid = &gridSub;
    }
    aPar->tune[AUDIO_TUNE_GAIN].grid      = &gridGain;
}

void tda731xSetTune(AudioTune tune, int8_t value)
{
    AudioRaw raw;
    audioSetRawBalance(&raw, 0, aPar->mode == AUDIO_MODE_2_1);

    switch (tune) {
    case AUDIO_TUNE_VOLUME:
        i2cBegin(I2C_AMP, TDA731X_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)(TDA731X_VOLUME | -value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BALANCE:
    case AUDIO_TUNE_FRONTREAR:
        i2cBegin(I2C_AMP, TDA731X_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)(TDA731X_SP_REAR_LEFT | -raw.rearLeft));
        i2cSend(I2C_AMP, (uint8_t)(TDA731X_SP_REAR_RIGHT | -raw.rearRight));
        i2cSend(I2C_AMP, (uint8_t)(TDA731X_SP_FRONT_LEFT | -raw.frontLeft));
        i2cSend(I2C_AMP, (uint8_t)(TDA731X_SP_FRONT_RIGHT | -raw.frontRight));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BASS:
        i2cBegin(I2C_AMP, TDA731X_I2C_ADDR);
        i2cSend(I2C_AMP, TDA731X_BASS | (uint8_t)(value > 0 ? 15 - value : 7 + value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_TREBLE:
        i2cBegin(I2C_AMP, TDA731X_I2C_ADDR);
        i2cSend(I2C_AMP, TDA731X_TREBLE | (uint8_t)(value > 0 ? 15 - value : 7 + value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_GAIN:
        tda731xSwitch(aPar->input, value, !!(aPar->flags & AUDIO_FLAG_LOUDNESS));
        break;
    default:
        break;
    }
}

void tda731xSetInput(int8_t value)
{
    tda731xSwitch(value, aPar->tune[AUDIO_TUNE_GAIN].value, !!(aPar->flags & AUDIO_FLAG_LOUDNESS));
}

void tda731xSetMute(bool value)
{
    if (value) {
        i2cBegin(I2C_AMP, TDA731X_I2C_ADDR);
        i2cSend(I2C_AMP, TDA731X_SP_REAR_LEFT | TDA731X_MUTE);
        i2cSend(I2C_AMP, TDA731X_SP_REAR_RIGHT | TDA731X_MUTE);
        i2cSend(I2C_AMP, TDA731X_SP_FRONT_LEFT | TDA731X_MUTE);
        i2cSend(I2C_AMP, TDA731X_SP_FRONT_RIGHT | TDA731X_MUTE);
        i2cTransmit(I2C_AMP);
    } else {
        tda731xSetTune(AUDIO_TUNE_BALANCE, aPar->tune[AUDIO_TUNE_VOLUME].value);
    }
}

void tda731xSetLoudness(bool value)
{
    tda731xSwitch(aPar->input, aPar->tune[AUDIO_TUNE_GAIN].value, value);
}

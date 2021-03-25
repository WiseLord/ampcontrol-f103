#include "pt232x.h"

#include "audio.h"
#include "hwlibs.h"
#include "i2c.h"

// I2C address
#define PT2322_I2C_ADDR         0x88
#define PT2323_I2C_ADDR         0x94

// I2C function selection
#define PT2322_NO_FUNC          0x00

#define PT2322_TRIM_FL          0x10
#define PT2322_TRIM_FR          0x20
#define PT2322_TRIM_CT          0x30
#define PT2322_TRIM_RL          0x40
#define PT2322_TRIM_RR          0x50
#define PT2322_TRIM_SB          0x60

#define PT2322_FUNCTION         0x70
#define PT2322_MUTE_ON          0x08
#define PT2322_3D_OFF           0x04
#define PT2322_TONE_OFF         0x02

#define PT2322_BASS             0x90
#define PT2322_MIDDLE           0xA0
#define PT2322_TREBLE           0xB0
#define PT2322_INPUT_SW         0xC7
#define PT2322_VOL_LO           0xD0
#define PT2322_VOL_HI           0xE0

#define PT2322_CREAR_REGS       0xFF

#define PT2323_INPUT_SWITCH     0xC0
#define PT2323_INPUT_ST1        0x0B
#define PT2323_INPUT_ST2        0x0A
#define PT2323_INPUT_ST3        0x09
#define PT2323_INPUT_ST4        0x08
#define PT2323_INPUT_6CH        0x07

#define PT2323_MUTE_FL          0xF0
#define PT2323_MUTE_FR          0xF2
#define PT2323_MUTE_CT          0xF4
#define PT2323_MUTE_SB          0xF6
#define PT2323_MUTE_RL          0xF8
#define PT2323_MUTE_RR          0xFA
#define PT2323_UNMUTE_ALL       0xFE
#define PT2323_MUTE_ALL         0xFF

#define PT2323_ENH_SURR         0xD0
#define PT2323_ENH_SURR_OFF     0x01
#define PT2323_MIX              0x90
#define PT2323_MIX_6DB          0x01

static const AudioGrid gridVolume     = {-79,  0, (int8_t)(1.00 * STEP_MULT)}; // -79..0dB with 1dB step
static const AudioGrid gridTone       = { -7,  7, (int8_t)(2.00 * STEP_MULT)}; // -14..14dB with 2dB step
static const AudioGrid gridBalance    = { -7,  7, (int8_t)(1.00 * STEP_MULT)}; // -7..7dB with 1dB step
static const AudioGrid gridCenterSub  = {-15,  0, (int8_t)(1.00 * STEP_MULT)}; // -15..0dB with 1dB step
static const AudioGrid gridGain       = {  0,  1, (int8_t)(6.00 * STEP_MULT)}; // 0..6dB with 6dB step

static AudioParam *aPar;

static const AudioApi pt232xApi = {
    .init = pt232xInit,

    .setTune = pt232xSetTune,
    .setInput = pt232xSetInput,

    .setMute = pt232xSetMute,
    .setSurround = pt232xSetSurround,
    .setEffect3d = pt232xSetEffect3D,
    .setBypass = pt232xSetBypass,
};

static void pt232xSetSndFunc(bool mute, bool effect3d, bool bypass)
{
    uint8_t sndFunc = PT2322_FUNCTION;

    if (mute) {
        sndFunc |= PT2322_MUTE_ON;
    }
    if (!effect3d) {
        sndFunc |= PT2322_3D_OFF;
    }
    if (bypass) {
        sndFunc |= PT2322_TONE_OFF;
    }

    i2cBegin(I2C_AMP, PT2322_I2C_ADDR);
    i2cSend(I2C_AMP, sndFunc);
    i2cTransmit(I2C_AMP);
}

static void pt2322SetSpeakers(void)
{
    AudioRaw raw;
    audioSetRawBalance(&raw, 0, false);

    i2cBegin(I2C_AMP, PT2322_I2C_ADDR);
    i2cSend(I2C_AMP, PT2322_TRIM_FL | (uint8_t)(-raw.frontLeft));
    i2cSend(I2C_AMP, PT2322_TRIM_FR | (uint8_t)(-raw.frontRight));
    i2cSend(I2C_AMP, PT2322_TRIM_CT | (uint8_t)(-aPar->tune[AUDIO_TUNE_CENTER].value));
    i2cSend(I2C_AMP, PT2322_TRIM_RL | (uint8_t)(-raw.rearLeft));
    i2cSend(I2C_AMP, PT2322_TRIM_RR | (uint8_t)(-raw.rearRight));
    i2cSend(I2C_AMP, PT2322_TRIM_SB | (uint8_t)(-aPar->tune[AUDIO_TUNE_SUBWOOFER].value));
    i2cTransmit(I2C_AMP);
}

const AudioApi *pt232xGetApi(void)
{
    return &pt232xApi;
}

void pt232xInit(AudioParam *param)
{
    aPar = param;

    aPar->tune[AUDIO_TUNE_VOLUME].grid    = &gridVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid      = &gridTone;
    aPar->tune[AUDIO_TUNE_MIDDLE].grid    = &gridTone;
    aPar->tune[AUDIO_TUNE_TREBLE].grid    = &gridTone;
    if (aPar->mode == AUDIO_MODE_4_0 ||
        aPar->mode == AUDIO_MODE_4_1 ||
        aPar->mode == AUDIO_MODE_5_1) {
        aPar->tune[AUDIO_TUNE_FRONTREAR].grid = &gridBalance;
    }
    aPar->tune[AUDIO_TUNE_BALANCE].grid   = &gridBalance;
    if (aPar->mode == AUDIO_MODE_5_1) {
        aPar->tune[AUDIO_TUNE_CENTER].grid    = &gridCenterSub;
    }
    if (aPar->mode == AUDIO_MODE_2_1 ||
        aPar->mode == AUDIO_MODE_4_1 ||
        aPar->mode == AUDIO_MODE_5_1) {
        aPar->tune[AUDIO_TUNE_SUBWOOFER].grid = &gridCenterSub;
    }
    aPar->tune[AUDIO_TUNE_GAIN].grid      = &gridGain;

    i2cBegin(I2C_AMP, PT2322_I2C_ADDR);
    i2cSend(I2C_AMP, PT2322_CREAR_REGS);
    i2cTransmit(I2C_AMP);

    i2cBegin(I2C_AMP, PT2322_I2C_ADDR);
    i2cSend(I2C_AMP, PT2322_INPUT_SW);
    i2cTransmit(I2C_AMP);

    i2cBegin(I2C_AMP, PT2323_I2C_ADDR);
    i2cSend(I2C_AMP, PT2323_UNMUTE_ALL);
    i2cTransmit(I2C_AMP);
}

void pt232xSetTune(AudioTune tune, int8_t value)
{
    switch (tune) {
    case AUDIO_TUNE_VOLUME:
        i2cBegin(I2C_AMP, PT2322_I2C_ADDR);
        i2cSend(I2C_AMP, PT2322_VOL_HI | (uint8_t)((-value) / 10));
        i2cSend(I2C_AMP, PT2322_VOL_LO | (uint8_t)((-value) % 10));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_BASS:
    case AUDIO_TUNE_MIDDLE:
    case AUDIO_TUNE_TREBLE:
        i2cBegin(I2C_AMP, PT2322_I2C_ADDR);
        i2cSend(I2C_AMP, (uint8_t)(PT2322_BASS + ((tune - AUDIO_TUNE_BASS) << 4)) |
                (uint8_t)(value > 0 ? 15 - value : 7 + value));
        i2cTransmit(I2C_AMP);
        break;
    case AUDIO_TUNE_FRONTREAR:
    case AUDIO_TUNE_BALANCE:
    case AUDIO_TUNE_CENTER:
    case AUDIO_TUNE_SUBWOOFER:
        pt2322SetSpeakers();
        break;
    case AUDIO_TUNE_GAIN:
        i2cBegin(I2C_AMP, PT2323_I2C_ADDR);
        i2cSend(I2C_AMP, PT2323_MIX | (uint8_t)value);
        i2cTransmit(I2C_AMP);
        break;
    default:
        break;
    }
}

void pt232xSetInput(int8_t value)
{
    i2cBegin(I2C_AMP, PT2323_I2C_ADDR);
    i2cSend(I2C_AMP, (uint8_t)(PT2323_INPUT_SWITCH | (PT2323_INPUT_ST1 - value)));
    i2cTransmit(I2C_AMP);
}

void pt232xSetMute(bool value)
{
    pt232xSetSndFunc(value, !!(aPar->flags & AUDIO_FLAG_EFFECT3D), !!(aPar->flags & AUDIO_FLAG_BYPASS));
}

void pt232xSetEffect3D(bool value)
{
    pt232xSetSndFunc(!!(aPar->flags & AUDIO_FLAG_MUTE), value, !!(aPar->flags & AUDIO_FLAG_BYPASS));
}

void pt232xSetBypass(bool value)
{
    pt232xSetSndFunc(!!(aPar->flags & AUDIO_FLAG_MUTE), !!(aPar->flags & AUDIO_FLAG_EFFECT3D), value);
}

void pt232xSetSurround(bool value)
{
    i2cBegin(I2C_AMP, PT2323_I2C_ADDR);
    i2cSend(I2C_AMP, PT2323_ENH_SURR | value ? 0 : PT2323_ENH_SURR_OFF);
    i2cTransmit(I2C_AMP);
}

#include "tda7418.h"

#include "../i2c.h"
#include "../pins.h"

// Number of inputs
#define TDA7418_IN_CNT              4

static const AudioGrid gridVolume    = {-79, 15, (uint8_t)(1.00 * 8)}; // -79..15dB with 1dB step
static const AudioGrid gridToneBal   = {-15, 15, (uint8_t)(1.00 * 8)}; // -15..15dB with 1dB step
static const AudioGrid gridSubwoofer = {-15,  0, (uint8_t)(1.00 * 8)}; // -15..0dB with 1dB step
static const AudioGrid gridGain      = {  0, 15, (uint8_t)(1.00 * 8)}; // 0..15dB with 1dB step

static AudioParam *aPar;

void tda7418Init(AudioParam *param)
{
    aPar = param;
    aPar->inCnt = TDA7418_IN_CNT;

    aPar->item[AUDIO_TUNE_VOLUME].grid    = &gridVolume;
    aPar->item[AUDIO_TUNE_BASS].grid      = &gridToneBal;
    aPar->item[AUDIO_TUNE_MIDDLE].grid    = &gridToneBal;
    aPar->item[AUDIO_TUNE_TREBLE].grid    = &gridToneBal;
    aPar->item[AUDIO_TUNE_PREAMP].grid    = &gridVolume;
    aPar->item[AUDIO_TUNE_FRONTREAR].grid = &gridToneBal;
    aPar->item[AUDIO_TUNE_BALANCE].grid   = &gridToneBal;
    aPar->item[AUDIO_TUNE_SUBWOOFER].grid = &gridSubwoofer;
    aPar->item[AUDIO_TUNE_GAIN].grid      = &gridGain;

    tda7418SetMute(true);
}

void tda7418SetTune(AudioTune tune, int8_t value)
{

}

void tda7418SetInput(uint8_t value)
{

}

void tda7418SetMute(bool value)
{

}

void tda7418SetLoudness(bool value)
{

}

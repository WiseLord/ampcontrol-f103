#include "tuner.h"

#include "../eemul.h"

static Tuner tuner;

static void tunerReadSettings()
{
    uint16_t eeData;

    eeData = eeRead(EE_TUNER_IC);
    tuner.ic = (eeData == EE_EMPTY ? TUNER_IC_RDA5807 : eeData);

}

void tunerInit()
{
    tunerReadSettings();
}

Tuner *tunerGet()
{
    return &tuner;
}

void tunerSetFreq(uint16_t value)
{
    if (value < tuner.freqMin)
        value = tuner.freqMin;
    else if (value > tuner.freqMax)
        value = tuner.freqMax;

    tuner.freq = value;
    tuner.freqRead = value;

    if (tuner.setFreq) {
        tuner.setFreq();
    }
}

void tunerSetFlag(TunerFlag flag, uint8_t value)
{
    if (value)
        tuner.flag |= flag;
    else
        tuner.flag &= ~flag;

    if (tuner.setFlag) {
        tuner.setFlag();
    }
}

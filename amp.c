#include "amp.h"

#include <stdbool.h>

#include "audio/audio.h"
#include "control.h"
#include "hwlibs.h"
#include "i2c.h"
#include "karadio.h"
#include "pins.h"
#include "settings.h"
#include "swtimers.h"
#include "tuner/tuner.h"

static Amp amp = {AMP_STATUS_STBY, 0x00};

static void inputDisable(void)
{
    AudioProc *aProc = audioGet();
    uint8_t input = aProc->par.input;

    switch (aProc->par.inType[input]) {
    case IN_TUNER:
        tunerSetPower(false);
        break;
    case IN_KARADIO:
        karadioSetEnabled(false);
        break;
    default:
        break;
    }

    // TODO: Power off current input device
}

static void inputEnable(void)
{
    Tuner *tuner = tunerGet();
    AudioProc *aProc = audioGet();
    uint8_t input = aProc->par.input;

    switch (aProc->par.inType[input]) {
    case IN_TUNER:
        tunerSetPower(true);
        tunerSetVolume(tuner->par.volume);
        tunerSetMute(false);
        tunerSetFreq(tuner->par.freq);
        break;
    case IN_KARADIO:
        karadioSetEnabled(true);
        break;
    default:
        break;
    }
}

static void handleInputStatus(void)
{
    I2cAddrIdx i2cAddrIdx = (I2cAddrIdx)settingsGet(PARAM_I2C_EXT_IN_STAT);

    if (i2cAddrIdx == I2C_ADDR_DISABLED || i2cAddrIdx >= I2C_ADDR_END) {
        return;
    }

    uint8_t i2cAddr = 0x40;

    if (i2cAddrIdx <= PCF8574_0x4E) {
        i2cAddr = 0x40 | (uint8_t)((i2cAddrIdx - PCF8574_0x40) << 1);
    } else if (i2cAddrIdx <= PCF8574A_0x7E) {
        i2cAddr = 0x70 | (uint8_t)((i2cAddrIdx - PCF8574A_0x70) << 1);
    }

    i2cBegin(I2C_AMP, i2cAddr);
    i2cSend(I2C_AMP, amp.inputStatus);
    i2cTransmit(I2C_AMP);
}

static void inputSetPower(bool value)
{
    AudioProc *aProc = audioGet();
    uint8_t input = aProc->par.input;

    if (value) {
        amp.inputStatus = (uint8_t)(1 << input);
    } else {
        amp.inputStatus = 0x00;
    }

    handleInputStatus();
}

Amp *ampGet(void)
{
    return &amp;
}

void ampInit(void)
{
    pinsSetMute(true);
    pinsSetStby(true);

    i2cInit(I2C_AMP, 100000);
    inputSetPower(false);    // Power off input device
    i2cDeInit(I2C_AMP);

    amp.status = AMP_STATUS_STBY;
    controlReportAmpStatus();
}

void ampExitStby(void)
{
    audioReadSettings();
    tunerReadSettings();

    pinsSetStby(false);     // Power on amplifier

    i2cInit(I2C_AMP, 100000);
    inputSetPower(true);    // Power on input device
    i2cDeInit(I2C_AMP);

    amp.status = AMP_STATUS_POWERED;
    swTimSet(SW_TIM_AMP_INIT, 600);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    screenSaveSettings();

    audioSetMute(true);
    audioSetPower(false);

    inputDisable();

    ampInit();
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        pinsHwResetI2c();
        i2cInit(I2C_AMP, 100000);

        audioInit();
        audioSetPower(true);
        tunerInit();

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 500);
        controlReportAll();
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();

        audioSetMute(false);

        amp.status = AMP_STATUS_ACTIVE;

        swTimSet(SW_TIM_INPUT_POLL, 100);
        break;
    }
}

void ampSetInput(uint8_t value)
{
    swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

    audioSetMute(true);

    inputDisable();
    inputSetPower(false);
    audioSetInput(value);
    inputSetPower(true);

    amp.status = AMP_STATUS_HW_READY;
    swTimSet(SW_TIM_AMP_INIT, 400);
}

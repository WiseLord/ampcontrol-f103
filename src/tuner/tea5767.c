#include "tea5767.h"
#include "tea5767_regs.h"

#include "hwlibs.h"
#include "i2c.h"

#define TEA5767_I2C_ADDR        0xC0

#define TEA5767_WRBUF_SIZE          5
#define TEA5767_RDBUF_SIZE          4

typedef enum {
    HILO_STATE_OK = 0,

    HILO_STATE_START,
    HILO_STATE_HIGH,
    HILO_STATE_LOW,

    HILO_STATE_END,
} HiLoState;

static uint8_t wrBuf[TEA5767_WRBUF_SIZE];
static uint8_t rdBuf[TEA5767_RDBUF_SIZE];

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi tea5767Api = {
    .init = tea5767Init,
    .setFreq = tea5767SetFreq,
    .seek = tea5767Seek,

    .setMute = tea5767SetMute,
    .setForcedMono = tea5767SetForcedMono,

    .setPower = tea5767SetPower,

    .updateStatus = tea5767UpdateStatus,
};

static HiLoState hiloState = HILO_STATE_OK;

static void tea5767WriteI2C(uint8_t bytes)
{
    uint8_t i;

    i2cBegin(I2C_AMP, TEA5767_I2C_ADDR);
    for (i = 0; i < bytes; i++)
        i2cSend(I2C_AMP, wrBuf[i]);
    i2cTransmit(I2C_AMP);
}

static void tea5767InitRegs(void)
{
    wrBuf[2] = TEA5767_HLSI | TEA5767_SSL_HIGH;
    if (tPar->flags & TUNER_PARAM_MONO)
        wrBuf[2] |= TEA5767_MS;

    wrBuf[3] = TEA5767_HCC | TEA5767_SNC | TEA5767_SMUTE | TEA5767_XTAL;

    switch (tPar->band) {
    case TUNER_BAND_FM_JAPAN:
        wrBuf[3] |= TEA5767_BL;
        tPar->fMin = 7600;
        tPar->fMax = 9100;
        break;
    default:
        wrBuf[3] &= ~TEA5767_BL;
        tPar->fMin = 8750;
        tPar->fMax = 10800;
        break;
    }

    tPar->fStep = 10;

    wrBuf[4] = 0;
    if (tPar->deemph == TUNER_DEEMPH_75u)
        wrBuf[4] |= TEA5767_DTC;
}

static void tea5767FreqToRegs(uint16_t freq)
{
    bool hlsi = ((wrBuf[2] & TEA5767_HLSI) == TEA5767_HLSI);

    const int32_t fRefs = 32768;
    const int32_t fIf = (hlsi ? 225000 : -225000);

    uint16_t pll = (uint16_t)((4 * (freq * 10000 + fIf) + fRefs / 2) / fRefs);

    wrBuf[0] &= 0xC0;
    wrBuf[0] |= (pll >> 8) & 0x3F;

    wrBuf[1] = pll & 0xFF;
}

static void tea5767HandleHiLoFreq(uint16_t freq)
{
    static uint8_t levelHigh = 0;

    switch (hiloState) {
    case HILO_STATE_START:
        // Check high freq
        wrBuf[2] |= TEA5767_HLSI;
        hiloState = HILO_STATE_HIGH;
        tea5767FreqToRegs(freq + 45);
        tea5767WriteI2C(3);
        break;
    case HILO_STATE_HIGH:
        // Store high rssi
        levelHigh = tStatus->rssi;
        // Check low freq
        wrBuf[2] &= ~TEA5767_HLSI;
        hiloState = HILO_STATE_LOW;
        tea5767FreqToRegs(freq - 45);
        tea5767WriteI2C(3);
        break;
    case HILO_STATE_LOW:
        // Make a choice
        if (levelHigh < tStatus->rssi) {
            wrBuf[2] |= TEA5767_HLSI;
        } else {
            wrBuf[2] &= ~TEA5767_HLSI;
        }
        hiloState = HILO_STATE_OK;
        tea5767FreqToRegs(freq);

        // Unmute output
        if (!(tPar->flags & TUNER_PARAM_MUTE)) {
            wrBuf[0] &= ~TEA5767_MUTE;
        }

        tea5767WriteI2C(3);
        break;
    default:
        tea5767WriteI2C(1);
        break;
    }
}

static uint16_t tea5767GetFreq(void)
{
    uint16_t pll = (uint16_t)(((rdBuf[0] & TEA5767_RD_PLL_13_8) << 8) |
                              (rdBuf[1] & TEA5767_RD_PLL_7_0));

    bool hlsi = ((wrBuf[2] & TEA5767_HLSI) == TEA5767_HLSI);

    const int32_t fRefs = 32768;
    const int32_t fIf = (hlsi ? 225000 : -225000);

    uint16_t freq = (uint16_t)((((pll * fRefs) / 4 - fIf) + 5000) / 10000);

    // Round to the nearest freq in 100kHz grid
    freq = ((freq + tPar->fStep / 2) / tPar->fStep) * 10;

    return freq;
}

const TunerApi *tea5767GetApi(void)
{
    return &tea5767Api;
}

void tea5767Init(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    tea5767InitRegs();
    tea5767WriteI2C(TEA5767_WRBUF_SIZE);
}

void tea5767SetFreq(uint16_t value)
{
    // Exit seek mode
    wrBuf[0] &= ~TEA5767_SM;

    // Mute output
    wrBuf[0] |= TEA5767_MUTE;

    hiloState = HILO_STATE_START;
    tea5767HandleHiLoFreq(value);
}

void tea5767Seek(int8_t direction)
{
    uint16_t freq = tea5767GetFreq();

    // Reset high/low injection state if seeking now
    hiloState = HILO_STATE_OK;

    // Enter seek mode
    tStatus->flags &= ~(TUNER_STATUS_SEEKUP | TUNER_STATUS_SEEKDOWN);
    wrBuf[0] |= TEA5767_SM;

    // Mute output
    wrBuf[0] |= TEA5767_MUTE;

    // Search direction
    if (direction > 0) {
        freq += tPar->fStep;
        wrBuf[2] |= TEA5767_SUD;
        tStatus->flags |= TUNER_STATUS_SEEKUP;
    } else {
        freq -= tPar->fStep;
        wrBuf[2] &= ~TEA5767_SUD;
        tStatus->flags |= TUNER_STATUS_SEEKDOWN;
    }

    if (freq < tPar->fMin) {
        tea5767SetFreq(tPar->fMax);
    } else if (freq > tPar->fMax) {
        tea5767SetFreq(tPar->fMin);
    } else {
        tea5767FreqToRegs(freq);
        tea5767WriteI2C(3);
    }
}

void tea5767SetMute(bool value)
{
    if (value)
        wrBuf[0] |= TEA5767_MUTE;
    else
        wrBuf[0] &= ~TEA5767_MUTE;

    tea5767WriteI2C(1);
}

void tea5767SetForcedMono(bool value)
{
    if (value)
        wrBuf[2] |= TEA5767_MS;
    else
        wrBuf[2] &= ~TEA5767_MS;

    tea5767WriteI2C(3);
}

void tea5767SetPower(bool value)
{
    if (value) {
        wrBuf[3] &= ~TEA5767_STBY;
    } else {
        wrBuf[3] |= TEA5767_STBY;
    }
    tea5767WriteI2C(4);
}

void tea5767UpdateStatus()
{
    i2cBegin(I2C_AMP, TEA5767_I2C_ADDR);
    i2cReceive(I2C_AMP, rdBuf, TEA5767_RDBUF_SIZE);

    if (hiloState == HILO_STATE_OK) {
        tStatus->freq = tea5767GetFreq();
    }

    tStatus->rssi = (rdBuf[3] & TEA5767_LEV) >> 4;

    tStatus->flags = TUNER_STATUS_INIT;

    if (rdBuf[2] & TEA5767_STEREO) {
        tStatus->flags |= TUNER_STATUS_STEREO;
    }
    if (rdBuf[0] & TEA5767_RF) {
        tStatus->flags |= TUNER_STATUS_READY;
    }
    if (rdBuf[0] & TEA5767_BLF) {
        tStatus->flags |= TUNER_STATUS_BANDLIM;
    }

    if (wrBuf[0] & TEA5767_SM) {
        if (tStatus->flags & TUNER_STATUS_READY) {
            tea5767SetFreq(tStatus->freq);
        } else {
            if (wrBuf[2] & TEA5767_SUD) {
                tStatus->flags |= TUNER_STATUS_SEEKUP;
            } else {
                tStatus->flags |= TUNER_STATUS_SEEKDOWN;
            }
        }
    } else {
        if (tStatus->flags & TUNER_STATUS_READY) {
            tea5767HandleHiLoFreq(tStatus->freq);
        }
    }
}

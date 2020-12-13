#include "si470x.h"
#include "si470x_regs.h"

#include "hwlibs.h"
#include "i2c.h"
#include "rds/parser.h"

#define SI470X_I2C_ADDR             0x20

#define SI470X_WRBUF_SIZE           12
#define SI470X_RDBUF_SIZE           12

static uint8_t wrBuf[SI470X_WRBUF_SIZE];
static uint8_t rdBuf[SI470X_RDBUF_SIZE];

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi si470xApi = {
    .init = si470xInit,
    .setFreq = si470xSetFreq,
    .seek = si470xSeek,

    .setVolume = si470xSetVolume,

    .setMute = si470xSetMute,
    .setForcedMono = si470xSetForcedMono,
    .setRds = si470xSetRds,

    .setPower = si470xSetPower,

    .updateStatus = si470xUpdateStatus,
};

static bool seeking = false;

static void si470xWriteI2C(uint8_t bytes)
{
    uint8_t i;

    i2cBegin(I2C_AMP, SI470X_I2C_ADDR);
    for (i = 0; i < bytes; i++)
        i2cSend(I2C_AMP, wrBuf[i]);
    i2cTransmit(I2C_AMP);
}

static void si470xInitRegs(void)
{
    wrBuf[0] = SI470X_SKMODE | SI470X_RDSM;

    wrBuf[4] = SI470X_RDS;
    if (tPar->deemph != TUNER_DEEMPH_75u) {
        wrBuf[4] |= SI470X_DE;  // 50us used in Europe
    }

    wrBuf[5] = SI470X_BLNDADJ_19_37;

    wrBuf[6] = SI470X_SEEKTH & 12; // 25 by default for backward compatibility

    wrBuf[7] = SI470X_VOLUME;
    switch (tPar->band) {
    case TUNER_BAND_FM_JAPAN:
        wrBuf[7] |= SI470X_BAND_JAPAN;
        tPar->fMin = 7600;
        tPar->fMax = 9100;
        break;
    case TUNER_BAND_FM_WORLDWIDE:
        wrBuf[7] |= SI470X_BAND_JAPAN_WIDE;
        tPar->fMin = 7600;
        tPar->fMax = 10800;
        break;
    default:
        wrBuf[7] |= SI470X_BAND_US_EUROPE;
        tPar->fMin = 8750;
        tPar->fMax = 10800;
        break;
    }
    switch (tPar->step) {
    case TUNER_STEP_50K:
        wrBuf[7] |= SI470X_SPACE_50;
        tPar->fStep = 5;
        break;
    case TUNER_STEP_200K:
        wrBuf[7] |= SI470X_SPACE_200;
        tPar->fStep = 20;
        break;
    default:
        wrBuf[7] |= SI470X_SPACE_100;
        tPar->fStep = 10;
        break;
    }

    wrBuf[9] = (SI470X_SKSNR & 0x10) | (SI470X_SKCNT & 0x01);

    wrBuf[10] = 0x01 | SI470X_XOSCEN;
    wrBuf[11] = 0x00;

    si470xWriteI2C(SI470X_WRBUF_SIZE);
}

static uint16_t si470xGetFreq()
{
    uint16_t chan = rdBuf[2] & SI740X_READCHAN_9_8;
    chan <<= 8;
    chan |= rdBuf[3];

    return chan * tPar->fStep + tPar->fMin;
}

const TunerApi *si470xGetApi(void)
{
    return &si470xApi;
}

void si470xInit(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    si470xInitRegs();
}

void si470xSetFreq(uint16_t value)
{
    uint16_t chan = (value - tPar->fMin) / tPar->fStep;

    // Exit seek mode
    wrBuf[0] &= ~SI470X_SEEK; // not seek
    seeking = false;

    wrBuf[2] = SI470X_TUNE | ((chan >> 8) & SI470X_CHAN_9_8);
    wrBuf[3] = (chan & SI470X_CHAN_7_0);

    si470xWriteI2C(4);

}

void si470xSeek(int8_t direction)
{
    // Enter seek mode
    tStatus->flags &= ~(TUNER_FLAG_SEEKUP | TUNER_FLAG_SEEKDOWN);
    wrBuf[0] |= SI470X_SEEK;
    seeking = true;

    if (direction > 0) {
        wrBuf[0] |= SI470X_SEEKUP;
        tStatus->flags |= TUNER_FLAG_SEEKUP;
    } else {
        wrBuf[0] &= ~SI470X_SEEKUP;
        tStatus->flags |= TUNER_FLAG_SEEKDOWN;
    }

    si470xWriteI2C(2);
}

void si470xSetVolume(int8_t value)
{
    wrBuf[7] &= ~SI470X_VOLUME;
    wrBuf[7] |= value;

    si470xWriteI2C(8);
}

void si470xSetMute(bool value)
{
    if (value) {
        wrBuf[0] &= ~SI470X_DMUTE;
    } else {
        wrBuf[0] |= SI470X_DMUTE;
    }

    si470xWriteI2C(2);
}

void si470xSetForcedMono(bool value)
{
    if (value) {
        wrBuf[0] |= SI470X_MONO;
    } else {
        wrBuf[0] &= ~SI470X_MONO;
    }

    si470xWriteI2C(2);
}

void si470xSetRds(bool value)
{
    if (value) {
        wrBuf[4] |= SI470X_RDS;
    } else {
        wrBuf[4] &= ~SI470X_RDS;
    }

    si470xWriteI2C(6);
}

void si470xSetPower(bool value)
{
    wrBuf[1] = SI470X_ENABLE;

    if (value) {
        wrBuf[1] &= ~SI470X_DISABLE;
    } else {
        wrBuf[1] |= SI470X_DISABLE;
    }

    si470xWriteI2C(2);
}

void si470xUpdateStatus(void)
{
    i2cBegin(I2C_AMP, SI470X_I2C_ADDR);
    i2cReceive(I2C_AMP, rdBuf, SI470X_RDBUF_SIZE);

    tStatus->freq = si470xGetFreq();
    tStatus->rssi = rdBuf[1] & SI740X_RSSI;

    if (rdBuf[0] & SI740X_ST) {
        tStatus->flags |= TUNER_FLAG_STEREO;
    }
    if (rdBuf[0] & SI740X_STC) {
        tStatus->flags |= TUNER_FLAG_READY;
    }
    if (rdBuf[0] & SI740X_SFBL) {
        tStatus->flags |= TUNER_FLAG_BANDLIM;
    }

    if (tPar->rds &&
        (rdBuf[0] & SI740X_RDSR) && (rdBuf[0] & SI740X_RDSS)) {
        if ((rdBuf[0] & SI740X_BLERA) != SI740X_BLERA &&
            (rdBuf[2] & SI740X_BLERB) != SI740X_BLERB &&
            (rdBuf[2] & SI740X_BLERC) != SI740X_BLERC &&
            (rdBuf[2] & SI740X_BLERD) != SI740X_BLERD) {

            RdsBlock rdsBlock = {
                .a = (rdBuf[4] << 8) | rdBuf[5],
                .b = (rdBuf[6] << 8) | rdBuf[7],
                .c = (rdBuf[8] << 8) | rdBuf[9],
                .d = (rdBuf[10] << 8) | rdBuf[11],
            };
            rdsParserDecode(&rdsBlock);

            tStatus->flags |= TUNER_FLAG_RDS_READY;
        }
    }

    if (seeking == true) {
        if (wrBuf[0] & SI470X_SEEKUP) {
            tStatus->flags |= TUNER_FLAG_SEEKUP;
        } else {
            tStatus->flags |= TUNER_FLAG_SEEKDOWN;
        }
    }

    if (tStatus->flags & TUNER_FLAG_READY) {
        seeking = false;
        wrBuf[0] &= ~SI470X_SEEK;
        wrBuf[2] &= ~SI470X_TUNE;
        si470xWriteI2C(4);
    }
}

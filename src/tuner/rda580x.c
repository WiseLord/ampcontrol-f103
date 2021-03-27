#include "rda580x.h"
#include "rda580x_regs.h"

#include "hwlibs.h"
#include "i2c.h"
#include "rds/parser.h"

#define RDA5807M_I2C_SEQ_ADDR       0x20
#define RDA5807M_I2C_RAND_ADDR      0x22

enum {
    REG_02h,
    REG_02l,
    REG_03h,
    REG_03l,
    REG_04h,
    REG_04l,
    REG_05h,
    REG_05l,
    REG_06h,
    REG_06l,
    REG_07h,
    REG_07l,
    REG_08h,
    REG_08l,

    REG_WR_END
};

enum {
    REG_02,
    REG_03,
    REG_04,
    REG_05,
    REG_06,
    REG_07,
    REG_08,
};

enum {
    REG_0Ah,
    REG_0Al,
    REG_0Bh,
    REG_0Bl,
    REG_0Ch,
    REG_0Cl,
    REG_0Dh,
    REG_0Dl,
    REG_0Eh,
    REG_0El,
    REG_0Fh,
    REG_0Fl,

    REG_RD_END
};

enum {
    REG_0A,
    REG_0B,
    REG_0C,
    REG_0D,
    REG_0E,
    REG_0F,
};

static uint8_t wrBuf[REG_WR_END];
static uint8_t rdBuf[REG_RD_END];

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi rda580xApi = {
    .init = rda580xInit,
    .setFreq = rda580xSetFreq,
    .seek = rda580xSeek,

    .setVolume = rda580xSetVolume,

    .setMute = rda580xSetMute,
    .setBassBoost = rda580xSetBassBoost,
    .setForcedMono = rda580xSetForcedMono,
    .setRds = rda580xSetRds,

    .setPower = rda580xSetPower,

    .updateStatus = rda580xUpdateStatus,
};

static bool seeking = false;

static void rda580xWriteReg(uint8_t reg)
{
    uint8_t *wrAddr = &wrBuf[2 * reg - 4];

    i2cBegin(I2C_AMP, RDA5807M_I2C_RAND_ADDR);
    i2cSend(I2C_AMP, reg);
    i2cSend(I2C_AMP, *wrAddr++);
    i2cSend(I2C_AMP, *wrAddr++);
    i2cTransmit(I2C_AMP);
}

static void rda580xSetBit(uint8_t reg, uint8_t bit, uint8_t cond)
{
    if (cond) {
        wrBuf[reg] |= bit;
    } else {
        wrBuf[reg] &= ~bit;
    }

    rda580xWriteReg(reg / 2 + 2);
}

static void rda580xInitRegs(void)
{
    wrBuf[REG_02h] = RDA580X_DHIZ;
    if (tPar->flags & TUNER_PARAM_BASS)
        wrBuf[REG_02h] |= RDA5807_BASS;
    if (tPar->flags & TUNER_PARAM_MONO)
        wrBuf[REG_02h] |= RDA580X_MONO;
    wrBuf[REG_02l] = RDA580X_SKMODE | RDA580X_CLK_MODE_32768 | RDA5807_NEW_METHOD;
    if (tPar->flags & TUNER_PARAM_RDS)
        wrBuf[REG_02l] |= RDA5807_RDS_EN;
    rda580xWriteReg(0x02);

    wrBuf[REG_03h] = 0x00;
    wrBuf[REG_03l] = 0x00;
    switch (tPar->band) {
    case TUNER_BAND_FM_JAPAN:
        wrBuf[REG_03l] |= RDA580X_BAND_JAPAN;
        tPar->fMin = 7600;
        tPar->fMax = 9100;
        break;
    case TUNER_BAND_FM_WORLDWIDE:
        wrBuf[REG_03l] |= RDA580X_BAND_WORLDWIDE;
        tPar->fMin = 7600;
        tPar->fMax = 10800;
        break;
    case TUNER_BAND_FM_EASTEUROPE:
        wrBuf[REG_03l] |= RDA580X_BAND_EASTEUROPE;
        tPar->fMin = 6500;
        tPar->fMax = 7600;
        break;
    default:
        wrBuf[REG_03l] |= RDA580X_BAND_US_EUROPE;
        tPar->fMin = 8700;
        tPar->fMax = 10800;
        break;
    }
    switch (tPar->step) {
    case TUNER_STEP_50K:
        wrBuf[REG_03l] |= RDA580X_SPACE_50;
        tPar->fStep = 5;
        break;
    case TUNER_STEP_200K:
        wrBuf[REG_03l] |= RDA580X_SPACE_200;
        tPar->fStep = 20;
        break;
    default:
        wrBuf[REG_03l] |= RDA580X_SPACE_100;
        tPar->fStep = 10;
        break;
    }
    rda580xWriteReg(0x03);

    wrBuf[REG_04h] = RDA580X_AFCD;
    if (tPar->deemph != TUNER_DEEMPH_75u)
        wrBuf[REG_04h] |= RDA580X_DE;
    wrBuf[REG_04l] |= RDA580X_GPIO3_ST_IND;
    rda580xWriteReg(0x04);

    wrBuf[REG_05h] = 0x08; // TODO: Handle seek threshold
    wrBuf[REG_05l] = RDA580X_LNA_PORT_SEL_LNAP;
    wrBuf[REG_05l] |= tPar->volume;
    rda580xWriteReg(0x05);

    // Nothing do with register 06 for now

    wrBuf[REG_07h] = RDA5807_TH_SOFRBLEND_DEF | RDA5807_65M_50M_MODE;
    wrBuf[REG_07l] = RDA5807_SEEK_TH_OLD_DEF | RDA5807_SOFTBLEND_EN; // TODO: Handle softblend
    rda580xWriteReg(0x07);
}

static uint16_t rda580xGetFreq(void)
{
    uint16_t chan = rdBuf[REG_0Ah] & RDA580X_READCHAN_9_8;
    chan <<= 8;
    chan |= rdBuf[REG_0Al];

    return chan * tPar->fStep + tPar->fMin;
}

const TunerApi *rda580xGetApi(void)
{
    return &rda580xApi;
}

void rda580xInit(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    rda580xInitRegs();
}

void rda580xSetFreq(uint16_t value)
{
    uint16_t chan = (value - tPar->fMin) / tPar->fStep;

    // Exit seek mode
    wrBuf[REG_02h] &= ~RDA580X_SEEK; // not seek
    seeking = false;

    wrBuf[REG_03h] = (chan >> 2) & RDA580X_CHAN_9_2;
    wrBuf[REG_03l] &= ~RDA580X_CHAN_1_0;
    wrBuf[REG_03l] |= (chan << 6) & RDA580X_CHAN_1_0;

    wrBuf[REG_03l] |= RDA580X_TUNE;
    rda580xWriteReg(0x03);
    wrBuf[REG_03l] &= ~RDA580X_TUNE;
}

void rda580xSeek(int8_t direction)
{
    // Enter seek mode
    tStatus->flags &= ~(TUNER_STATUS_SEEKUP | TUNER_STATUS_SEEKDOWN);
    wrBuf[REG_02h] |= RDA580X_SEEK;
    seeking = true;

    if (direction > 0) {
        tStatus->flags |= TUNER_STATUS_SEEKUP;
    } else {
        tStatus->flags |= TUNER_STATUS_SEEKDOWN;
    }

    wrBuf[REG_02h] |= RDA580X_SEEK;
    rda580xSetBit(REG_02h, RDA580X_SEEKUP, direction > 0);
    wrBuf[REG_02h] &= ~RDA580X_SEEK;
}

void rda580xSetVolume(int8_t value)
{
    if (value < RDA580X_VOL_MIN)
        value = RDA580X_VOL_MIN;
    if (value > RDA580X_VOL_MAX)
        value = RDA580X_VOL_MAX;

    wrBuf[REG_05l] &= ~RDA580X_VOLUME;
    wrBuf[REG_05l] |= value;

    rda580xWriteReg(0x05);
}

void rda580xSetMute(bool value)
{
    rda580xSetBit(REG_02h, RDA580X_DMUTE, !value);
}

void rda580xSetBassBoost(bool value)
{
    rda580xSetBit(REG_02h, RDA5807_BASS, value);
}

void rda580xSetForcedMono(bool value)
{
    rda580xSetBit(REG_02h, RDA580X_MONO, value);
}

void rda580xSetRds(bool value)
{
    rda580xSetBit(REG_02l, RDA5807_RDS_EN, value);
}

void rda580xSetPower(bool value)
{
    rda580xSetBit(REG_02l, RDA580X_ENABLE, value);
}

void rda580xUpdateStatus()
{
    uint8_t buf[1 * REG_RD_END];

    i2cBegin(I2C_AMP, RDA5807M_I2C_SEQ_ADDR);
    i2cReceive(I2C_AMP, buf, sizeof(buf));

    for (uint8_t reg = REG_0Ah; reg < REG_RD_END; reg++) {
        rdBuf[reg] = buf[reg];
    }

    tStatus->freq = rda580xGetFreq();
    tStatus->rssi = (rdBuf[REG_0Bh] & RDA580X_RSSI) >> 2;

    if (rdBuf[REG_0Ah] & RDA580X_ST) {
        tStatus->flags |= TUNER_STATUS_STEREO;
    }
    if (rdBuf[REG_0Bl] & RDA580X_FM_READY) {
        tStatus->flags |= TUNER_STATUS_READY;
        seeking = false;
    }
    if (rdBuf[REG_0Ah] & RDA580X_SF) {
        tStatus->flags |= TUNER_STATUS_BANDLIM;
    }

    if ((tPar->flags & TUNER_PARAM_RDS) &&
        (rdBuf[REG_0Ah] & RDA5807_RDSR) && (rdBuf[REG_0Ah] & RDA5807_RDSS)) {
        // If there are no non-correctable errors in blocks A-D
        if ((rdBuf[REG_0Bl] & RDA5807_BLERA) != RDA5807_BLERA &&
            (rdBuf[REG_0Bl] & RDA5807_BLERB) != RDA5807_BLERB ) {

            RdsBlock rdsBlock = {
                .a = (rdBuf[REG_04h] << 8) | rdBuf[REG_04l],
                .b = (rdBuf[REG_05h] << 8) | rdBuf[REG_05l],
                .c = (rdBuf[REG_06h] << 8) | rdBuf[REG_06l],
                .d = (rdBuf[REG_07h] << 8) | rdBuf[REG_07l],
            };
            rdsParserDecode(&rdsBlock);
        }
    }

    if (seeking == true) {
        if (wrBuf[REG_02h] & RDA580X_SEEKUP) {
            tStatus->flags |= TUNER_STATUS_SEEKUP;
        } else {
            tStatus->flags |= TUNER_STATUS_SEEKDOWN;
        }
    }
}

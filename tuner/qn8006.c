#include "qn8006.h"

#include "qn8006_regs.h"
#include "../i2c.h"
#include "../pins.h"

#define QN8006_I2C_ADDR        0x56

#define QN8006_BUF_SIZE         40

static uint8_t buf[QN8006_BUF_SIZE];

static TunerParam *tPar;
static TunerStatus *tStatus;

static void qn8006WriteReg(uint8_t reg, uint8_t num)
{
    i2cBegin(I2C_AMP, QN8006_I2C_ADDR);
    i2cSend(I2C_AMP, reg);
    for (uint8_t i = 0; i < num; i++) {
        i2cSend(I2C_AMP, buf[reg + i]);
    }
    i2cTransmit(I2C_AMP, true);
}

static void qn8006ReadReg(uint8_t reg, uint8_t num)
{
    i2cBegin(I2C_AMP, QN8006_I2C_ADDR);
    i2cSend(I2C_AMP, reg);
    i2cTransmit(I2C_AMP, false);

    i2cBegin(I2C_AMP, QN8006_I2C_ADDR | I2C_READ);
    i2cReceive(I2C_AMP, &buf[reg], num);
}

static void qn8006SwReset(void)
{
    buf[1] = QN8006_SWRST | QN8006_RECAL;

    qn8006WriteReg(1, 1);

    buf[1] = 0;
}


static void qn8006InitRegs(void)
{
    tPar->fMin = 7600;
    tPar->fMax = 10800;
    tPar->fStep = 5;

    buf[0] = QN8006_RXREQ | QN8006_CCA_CH_DIS;
    if (tPar->rds) {
        buf[0] |= QN8006_RDSEN;
    }

    buf[1] = 0;
    if (tPar->forcedMono) {
        buf[1] |= QN8006_FORCE_MO;
    }
    if (tPar->deemph == TUNER_DEEMPH_75u) {
        buf[1] |= QN8006_TC;
    }

    buf[2] = 0x2A;

    buf[3] = QN8006_XSEL_26M;

    buf[4] = (QN8006_SCSEL & 0x20);

    qn8006WriteReg(0, 4);
}


void qn8006Init(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    qn8006SwReset();

    qn8006InitRegs();
}

void qn8006SetFreq(uint16_t value)
{
    uint16_t chan = (value - tPar->fMin) / 5;

    buf[0x0B] &= ~QN8006_CH_9_8;
    buf[0x0B] |= (chan >> 8) | QN8006_CH_9_8;

    qn8006WriteReg(0x0B, 1);

    buf[0x08] = chan & QN8006_CH_7_0;
    qn8006WriteReg(0x08, 1);
}

void qn8006Seek(int8_t direction)
{

}

void qn8006SetMute(bool value)
{
    if (value)
        buf[0x03] |= QN8006_MUTE_EN;
    else
        buf[0x03] &= ~QN8006_MUTE_EN;

    qn8006WriteReg(0x03, 1);
}

void qn8006SetPower(bool value)
{
    if (value)
        buf[0x00] &= ~QN8006_STNBY;
    else
        buf[0x00] |= QN8006_STNBY;

    qn8006WriteReg(0x00, 1);
}

void qn8006UpdateStatus()
{
    qn8006ReadReg(0x1A, 1);

    if (!(buf[0x1A] & QN8006_ST_MO_RX)) {
        tStatus->flags |= TUNER_FLAG_STEREO;
    }
}

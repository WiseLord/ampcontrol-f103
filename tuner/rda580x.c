#include "rda580x.h"

#include "rda580x_regs.h"
#include "../i2c.h"

#define RDA5807M_I2C_SEQ_ADDR       0x20
#define RDA5807M_I2C_RAND_ADDR      0x22

#define RDA5807_WRBUF_SIZE          14
#define RDA5807_RDBUF_SIZE          12

static uint8_t wrBuf[RDA5807_WRBUF_SIZE];
static uint8_t rdBuf[RDA5807_RDBUF_SIZE];

static void rda580xWriteReg(uint8_t reg)
{
    uint8_t *wrAddr = &wrBuf[2 * reg - 4];

    i2cBegin(I2C_AMP, RDA5807M_I2C_RAND_ADDR);
    i2cSend(I2C_AMP, reg);
    i2cSend(I2C_AMP, *wrAddr++);
    i2cSend(I2C_AMP, *wrAddr++);
    i2cTransmit(I2C_AMP);
}

static void rda580xSetBit(uint8_t idx, uint8_t bit, uint8_t cond)
{
    if (cond) {
        wrBuf[idx] |= bit;
    } else {
        wrBuf[idx] &= ~bit;
    }

    rda580xWriteReg(idx / 2 + 2);
}

static void rda580xInitRegs(void)
{
    wrBuf[0] = RDA580X_DHIZ;
    wrBuf[1] = RDA580X_CLK_MODE_32768 | RDA580X_ENABLE;

    wrBuf[2] = 0x00;
    wrBuf[3] = RDA580X_BAND_US_EUROPE | RDA580X_SPACE_100;

    wrBuf[7] = RDA580X_LNA_PORT_SEL_LNAP;
}

void rda580xInit(void)
{
    rda580xInitRegs();

    rda580xSetPower(1);

    rda580xSetMute(0);
    rda580xSetVolume(0);

    rda580xSetFreq(9890);
}

void rda580xSetFreq(uint16_t value)
{
    uint16_t chan = (value - 8700) / 10;

    wrBuf[2] = (chan >> 2) & RDA580X_CHAN_9_2;
    wrBuf[3] &= ~RDA580X_CHAN_1_0;
    wrBuf[3] |= (chan << 6) & RDA580X_CHAN_1_0;

    wrBuf[3] |= RDA580X_TUNE;
    rda580xWriteReg(0x03);
    wrBuf[3] &= ~RDA580X_TUNE;
}

void rda580xSeek(int8_t direction)
{
    wrBuf[0] |= RDA580X_SEEK;
    rda580xSetBit(0, RDA580X_SEEKUP, direction > 0);
    wrBuf[0] &= ~RDA580X_SEEK;
}

void rda580xSetMute(uint8_t value)
{
    rda580xSetBit(0, RDA580X_DMUTE, !value);
}

void rda580xSetVolume(int8_t value)
{
    if (value < RDA580X_VOL_MIN)
        value = RDA580X_VOL_MIN;
    if (value > RDA580X_VOL_MAX)
        value = RDA580X_VOL_MAX;

    wrBuf[7] &= ~RDA580X_VOLUME;
    wrBuf[7] |= value;

    rda580xWriteReg(0x05);
}

void rda580xSetPower(uint8_t value)
{
    rda580xSetBit(1, RDA580X_ENABLE, value);
}

void rda580xUpdateStatus()
{
    i2cBegin(I2C_AMP, RDA5807M_I2C_SEQ_ADDR);
//    i2cReceive(I2C_AMP, rdBuf, RDA5807_RDBUF_SIZE);
    i2cReceive(I2C_AMP, rdBuf, 2);
}

uint16_t rda580xGetFreq()
{
    uint16_t chan = rdBuf[0] & RDA580X_READCHAN_9_8;
    chan <<= 8;
    chan |= rdBuf[1];

    return chan * 10 + 8700;
}

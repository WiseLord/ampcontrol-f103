#include "tea5767.h"

#include "tea5767_regs.h"
#include "../i2c.h"
#include "../pins.h"

#define TEA5767_I2C_ADDR        0xC0

#define TEA5767_WRBUF_SIZE          5
#define TEA5767_RDBUF_SIZE          5

static uint8_t wrBuf[TEA5767_WRBUF_SIZE];
static uint8_t rdBuf[TEA5767_RDBUF_SIZE];

static TunerParam *tPar;

//static uint8_t ctrl = 0x5E;

static void tea5767WriteI2C()
{
    uint8_t i;

    i2cBegin(I2C_AMP, TEA5767_I2C_ADDR);
    for (i = 0; i < TEA5767_WRBUF_SIZE; i++)
        i2cSend(I2C_AMP, wrBuf[i]);
    i2cTransmit(I2C_AMP, true);
}

static void tea5767InitRegs(void)
{
    wrBuf[2] = TEA5767_HLSI | TEA5767_SSL_HIGH;
    wrBuf[3] = TEA5767_HCC | TEA5767_SNC | TEA5767_SMUTE | TEA5767_XTAL;
    wrBuf[4] = 0;
}

void tea5767Init(TunerParam *param)
{
    tPar = param;

    tea5767InitRegs();
}

void tea5767SetFreq(uint16_t value)
{
    uint16_t pll = value * 4 + 90;

//    if (ctrl & TEA5767_XTAL) {
    pll = pll * 10000UL / 32768;
//    } else {
//        pll = pll / 5;
//    }

    wrBuf[0] &= 0xC0;
    wrBuf[0] |= (pll >> 8) & 0x3F;

    wrBuf[1] = pll & 0xFF;

    if (tPar->flags & TUNER_FLAG_MONO)
        wrBuf[2] |= TEA5767_MS;
    else
        wrBuf[2] &= ~TEA5767_MS;

    tea5767WriteI2C();
}

void tea5767Seek(int8_t direction)
{
    if (direction > 0) {
        wrBuf[2] |= TEA5767_SUD;
    } else {
        wrBuf[2] &= ~TEA5767_SUD;
    }
    wrBuf[0] |= TEA5767_SM;

    tea5767SetFreq(tea5767GetFreq());
}

void tea5767SetMute(bool value)
{
    if (value)
        wrBuf[0] |= TEA5767_MUTE;
    else
        wrBuf[0] &= ~TEA5767_MUTE;

    tea5767WriteI2C();
}

void tea5767SetPower(bool value)
{
    if (value) {
        wrBuf[3] &= ~TEA5767_STBY;
    } else {
        wrBuf[3] |= TEA5767_STBY;
    }
    tea5767WriteI2C();
}

void tea5767UpdateStatus()
{
    i2cBegin(I2C_AMP, TEA5767_I2C_ADDR);
    i2cReceive(I2C_AMP, rdBuf, TEA5767_RDBUF_SIZE);
}

uint16_t tea5767GetFreq()
{
    uint16_t pll = rdBuf[0] & 0x3F;
    pll <<= 8;
    pll |= rdBuf[1];

    pll = (pll * 32768UL / 10000 - 90 + 2) / 4;

    return pll;
}

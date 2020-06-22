#include "tux03x.h"

#include "hwlibs.h"
#include "i2c.h"

#define TUX03X_I2C_ADDR         0xC2
#define TUX03X_WRBUF_SIZE       10

static uint8_t wrBuf[TUX03X_WRBUF_SIZE];

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi tux03xApi = {
    .init = tux03xInit,
    .setFreq = tux03xSetFreq,

    .setForcedMono = tux03xSetForcedMono,

    .setPower = tux03xSetPower,

    .updateStatus = tux03xUpdateStatus,
};

static void tux03xWriteI2C(uint8_t bytes)
{
    uint8_t i;

    i2cBegin(I2C_AMP, TUX03X_I2C_ADDR);
    for (i = 0; i < bytes; i++) {
        i2cSend(I2C_AMP, wrBuf[i]);
    }
    i2cTransmit(I2C_AMP);
}


const TunerApi *tux03xGetApi()
{
    return &tux03xApi;
}

void tux03xInit(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    wrBuf[3] = 0x64;
    wrBuf[4] = 0xB1;
    wrBuf[5] = 0xC6;
    wrBuf[6] = 0x4B;
    wrBuf[7] = 0xA2;
    wrBuf[8] = 0xD2;
//    wrBuf[9] = ;

    tux03xSetPower(false);
}

void tux03xSetFreq(uint16_t value)
{
    uint16_t freq = value / 5 + 214;    // 9950

    wrBuf[0] = 0x80;
    wrBuf[1] = freq >> 8;
    wrBuf[2] = freq & 0xFF;

    tux03xWriteI2C(TUX03X_WRBUF_SIZE);
}

void tux03xSetForcedMono(bool value)
{
    if (value) {
        wrBuf[0] = 0x87;
        wrBuf[1] = 0x52;
    } else {
        wrBuf[0] = 0x87;
        wrBuf[1] = 0xd2;
    }
    tux03xWriteI2C(2);
}

void tux03xSetPower(bool value)
{
    if (value) {
        wrBuf[0] = 0x82;
        wrBuf[1] = 0x64;
    } else {
        wrBuf[0] = 0x82;
        wrBuf[1] = 0x00;
    }
    tux03xWriteI2C(2);
}

void tux03xUpdateStatus()
{

}

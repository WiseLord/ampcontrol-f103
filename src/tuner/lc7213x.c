#include "lc7213x.h"
#include "lc7213x_regs.h"

#include "hwlibs.h"
#include "rds/demod.h"
#include "tuner.h"
#include "utils.h"

// IF = 10.7MHz and step = 50kHz
#define LC7213X_IF                  1070
#define LC7213X_RF                  5 // 25kHz * 2 (for FMIN)

// I/O mode address
#define LC7213X_IO_IN1              0x28
#define LC7213X_IO_IN2              0x29
#define LC7213X_IO_OUT              0x2A

// Software SPI
#define SPISW_DI_Port          GPIOA
#define SPISW_DI_Pin           LL_GPIO_PIN_7
#define SPISW_DO_Port          GPIOA
#define SPISW_DO_Pin           LL_GPIO_PIN_6
#define SPISW_CLK_Port         GPIOA
#define SPISW_CLK_Pin          LL_GPIO_PIN_5
#define SPISW_CE_Port          GPIOC
#define SPISW_CE_Pin           LL_GPIO_PIN_13

static void SPIswInitPins(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = SPISW_DI_Pin;
    LL_GPIO_Init(SPISW_DI_Port, &initDef);
    initDef.Pin = SPISW_CLK_Pin;
    LL_GPIO_Init(SPISW_CLK_Port, &initDef);
    initDef.Pin = SPISW_CE_Pin;
    LL_GPIO_Init(SPISW_CE_Port, &initDef);

    LL_GPIO_SetPinMode(SPISW_DO_Port, SPISW_DO_Pin, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(SPISW_DO_Port, SPISW_DO_Pin, LL_GPIO_PULL_UP);
}

void SPIswSendByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 0x01) {
            SET(SPISW_DI);
        } else {
            CLR(SPISW_DI);
        }

        utiluDelay(1);
        SET(SPISW_CLK);
        utiluDelay(1);
        CLR(SPISW_CLK);

        data >>= 1;
    }
}

uint8_t SPIswReceiveByte(void)
{
    uint8_t data = 0;

    for (uint8_t i = 0; i < 8; i++) {
        utiluDelay(1);
        SET(SPISW_CLK);

        data >>= 1;

        if (READ(SPISW_DO)) {
            data |= 0x80;
        } else {
            data &= ~0x80;
        }

        utiluDelay(1);
        CLR(SPISW_CLK);
    }
    return data;
}

static uint8_t in1Buf[3] = {0x00, 0x00, 0x00};
static uint8_t in2Buf[3] = {0x00, 0x00, 0x00};

static uint8_t outBuf;
static uint8_t DO;

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi lc7213xApi = {
    .init = lc7213xInit,
    .setFreq = lc7213xSetFreq,

    .setMute = lc7213xSetMute,

    .updateStatus = lc7213xUpdateStatus,
};

static void updateIn1(void)
{
    SPIswSendByte(LC7213X_IO_IN1);
    SET(SPISW_CE);
    for (uint8_t i = 0; i < sizeof(in1Buf); i++) {
        SPIswSendByte(in1Buf[i]);
    }
    CLR(SPISW_CE);
}

static void updateIn2(void)
{
    SPIswSendByte(LC7213X_IO_IN2);
    SET(SPISW_CE);
    for (uint8_t i = 0; i < sizeof(in2Buf); i++) {
        SPIswSendByte(in2Buf[i]);
    }
    CLR(SPISW_CE);
}

static void readOut(void)
{
    DO = READ(SPISW_DO) ? 1 : 0;
    SPIswSendByte(LC7213X_IO_OUT);
    SET(SPISW_CE);
    outBuf = SPIswReceiveByte();
    CLR(SPISW_CE);
}

const TunerApi *lc7213xGetApi(void)
{
    return &lc7213xApi;
}

void lc7213xInit(TunerParam *param, TunerStatus *status)
{
    rdsDemodInit();

    tPar = param;
    tStatus = status;

    tPar->fMin = 8700;
    tPar->fMax = 10800;
    tPar->fStep = 10;

    SPIswInitPins();

    in1Buf[2] &= ~LC7213X_IN1_XS;       // Use 4.5MHz crystal
    in1Buf[2] |= LC7213X_IN1_FREF_25K;  // Use 25kHz base

    in2Buf[0] &= ~LC7213X_IN2_IOC1;     // Use IO1 as input
    in2Buf[0] &= ~LC7213X_IN2_IOC2;     // Use IO2 as input
    in2Buf[0] &= ~LC7213X_IN2_BO1;      // Set FM mode
    in2Buf[0] &= ~LC7213X_IN2_BO2;      // Set mute

    in2Buf[1] &= ~LC7213X_IN2_BO5;      // Set stereo mode
    in2Buf[1] |= LC7213X_IN2_UL_0;      // Unlock detection param
    in2Buf[1] |= LC7213X_IN2_DO_END_UC; // DO shows end of counter measure

    in2Buf[2] |= LC7213X_IN2_IFS;       // High sensitivity

    updateIn1();
    updateIn2();
}

void lc7213xSetFreq(uint16_t freq)
{
    uint16_t div;

    div = (freq + LC7213X_IF) / LC7213X_RF;

    in1Buf[0] = div & 0x00FF;
    in1Buf[1] = (div & 0xFF00) >> 8;

    in1Buf[2] |= LC7213X_IN1_DVS;   // Use FM input

    updateIn1();

    tunerGet()->status.freq = freq;
}

void lc7213xSetMute(bool value)
{
    if (value) {
        in2Buf[0] &= ~LC7213X_IN2_BO2;
    } else {
        in2Buf[0] |= LC7213X_IN2_BO2;
    }
    updateIn2();
}

void lc7213xSetForcedMono(bool value)
{
    if (value) {
        in2Buf[0] |= LC7213X_IN2_BO5;
    } else {
        in2Buf[0] &= ~LC7213X_IN2_BO5;
    }
    updateIn2();
}

void lc7213xUpdateStatus(void)
{
    readOut();
    if (!(outBuf & LC7213X_OUT_I2)) {
        tStatus->flags |= TUNER_FLAG_STEREO;
    }
    if (!(outBuf & LC7213X_OUT_I1)) {
        tStatus->flags |= TUNER_FLAG_STATION;
    }
    updateIn2();
}

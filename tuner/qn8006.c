#include "qn8006.h"

static TunerParam *tPar;
static TunerStatus *tStatus;

static void qn8006InitRegs(void)
{

}


void qn8006Init(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    qn8006InitRegs();
}

void qn8006SetFreq(uint16_t value)
{

}

void qn8006Seek(int8_t direction)
{

}

void qn8006SetMute(bool value)
{

}

void qn8006SetPower(bool value)
{

}

void qn8006UpdateStatus()
{

}

#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../../../audio/audiodefs.h"
#include "../../../display/dispdefs.h"
#include "../../../display/dispdrv.h"
#include "../../../tuner/rds.h"
#include "../../../tuner/stations.h"
#include "../../../tuner/tuner.h"
#include "../../../karadio.h"
#include "../../../pins.h"
#include "../../../rc.h"
#include "../../../settings.h"
#include "../../../spectrum.h"
#include "../../../usb/usbhid.h"

#include "emuldisp_stub.h"

static Spectrum spectrum;
static RcData rcData;
static Rds rds = {
    .PI = 0xDEAD,

    .TP = 0,
    .PTY = 15,

    .TA = 0,
    .MS = 1,
    .DI_ST = 1,
    .DI_AH = 0,
    .DI_CMP = 0,
    .DI_stPTY = 0,

    .PS = "My Radio",
    .text = "RDS text transmitted by the station",
};

const DispDriver dispdrv = {
    .width = EMUL_DISP_WIDTH,
    .height = EMUL_DISP_HEIGHT,
};

static KaRadioData krData = {"Station name", "Artist - title", KARADIO_FLAG_ALL, true};

static Station st[STATION_COUNT] = {
    [0] = {8750, "Relax FM"},
    [1] = {9240, "Радио-Минск"},
    [2] = {9370, "Юмор FM"},
    [3] = {9410, "Легенды FM"},
    [4] = {9620, "Мелодии Века"},
    [5] = {9740, "Минская Волна"},
    [6] = {9800, "Авторадио"},
    [7] = {9840, "Новое радио"},
    [8] = {9890, "Русское радио"},
    [9] = {9950, "Радио Юнистар"},
    [10] = {10040, "Хит-радио"},
    [11] = {10120, "Пилот FM"},
    [12] = {10170, "Центр FM"},
    [13] = {10210, "Радио РОКС"},
    [14] = {10250, "Народное радио"},
    [15] = {10290, "Культура"},
    [16] = {10370, "Радиус-FM"},
    [17] = {10460, "Радио Би-Эй"},
    [18] = {10510, "Столица"},
    [19] = {10570, "Душевное радио"},
    [20] = {10620, "1 национальный"},
    [21] = {10710, "Радио Мир"},
    [22] = {10790, "Альфа радио"},
};

void *eeGetPageAddr(uint16_t page)
{
    (void)page;

    return st;
}

void eeErasePages(uint16_t page, uint16_t count)
{
    (void)count;
    (void)page;

    memset(st, 0, sizeof(st));
}

void eeWritePage(uint16_t page, void *addr, uint16_t bytes)
{
    (void)page;

    memcpy(st, addr, bytes);
}

uint16_t rcGetCode(RcCmd cmd)
{
    (void)cmd;
    return 0;
}

void rcSaveCode(uint16_t cmd, uint16_t value)
{
    (void)cmd, (void)value;
}

RcData rcRead(bool clear)
{
    (void)clear;
    return rcData;
}

RcCmd rcGetCmd(RcData *rcData)
{
    (void) rcData;
    return RC_CMD_END;
}

void pinsSetMuteStby(bool value)
{
    (void)value;
}

void pinsSetMute(bool value)
{
    (void)value;
}

void pinsSetStby(bool value)
{
    (void)value;
}

bool pinsGetMuteStby(void)
{
    return true;
}

void pinsDeInitAmpI2c(void)
{
}

void pinsInitAmpI2c(void)
{
}

void pinsInitMuteStby(MuteStby value)
{
    (void)value;
}

Spectrum *spGet(void)
{
    return &spectrum;
}

void spGetADC(Spectrum *sp)
{
    for (uint8_t i = 0; i < SPECTRUM_SIZE; i++) {
        sp->data[SP_CHAN_LEFT].raw[i] = (rand() & 0xFF) * (rand() & 0xFF) / 256;
        sp->data[SP_CHAN_RIGHT].raw[i] = (rand() & 0xFF) * (rand() & 0xFF) / 256;
    }
}

void rdsReset(void)
{

}

bool rdsGetFlag(void)
{
    return tunerGet()->par.rds;
}

Rds *rdsGet(void)
{
    return &rds;
}

void usbHidSendKey(HidKey key)
{
    (void)key;
}

void usbHidSendMediaKey(HidMediaKey key)
{
    (void)key;
}

KaRadioData *karadioGet(void)
{
    return &krData;
}
void karadioUpdateStatus(void)
{
}

void karadioSendMediaCmd(uint8_t cmd)
{
    (void)cmd;
}

void eeInit()
{
}

void karadioSetEnabled(bool value)
{
    krData.playing = value;
}

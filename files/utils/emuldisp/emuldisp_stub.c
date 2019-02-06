#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../../../audio/audiodefs.h"
#include "../../../tuner/stations.h"
#include "../../../tuner/tunerdefs.h"
#include "../../../eemap.h"
#include "../../../spectrum.h"

static Spectrum spectrum;
static RcData rcData;

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
    memset(st, 0, sizeof(st));
}

void eeWritePage(uint16_t page, void *addr, uint16_t bytes)
{
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

Spectrum *spGet(void)
{
    return &spectrum;
}

void spGetADC(uint8_t *dataL, uint8_t *dataR)
{
    for (uint8_t i = 0; i < SPECTRUM_SIZE; i++) {
        dataL[i] = rand() & 0xFF;
        dataR[i] = rand() & 0xFF;
    }
}

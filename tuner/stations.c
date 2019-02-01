#include "stations.h"

#include "tuner.h"
#include "../eemul.h"

//static Station st[STATION_COUNT] = {
//    [0] = {8750, "Relax FM"},
//    [1] = {9240, "Радио-Минск"},
//    [2] = {9370, "Юмор FM"},
//    [3] = {9410, "Легенды FM"},
//    [4] = {9620, "Мелодии Века"},
//    [5] = {9740, "Минская Волна"},
//    [6] = {9800, "Авторадио"},
//    [7] = {9840, "Новое радио"},
//    [8] = {9890, "Русское радио"},
//    [9] = {9950, "Радио Юнистар"},
//    [10] = {10040, "Хит-радио"},
//    [11] = {10120, "Пилот FM"},
//    [12] = {10170, "Центр FM"},
//    [13] = {10210, "Радио РОКС"},
//    [14] = {10250, "Народное радио"},
//    [15] = {10290, "Культура"},
//    [16] = {10370, "Радиус-FM"},
//    [17] = {10460, "Радио Би-Эй"},
//    [18] = {10510, "Столица"},
//    [19] = {10570, "Душевное радио"},
//    [20] = {10620, "1 национальный"},
//    [21] = {10710, "Радио Мир"},
//    [22] = {10790, "Альфа радио"},
//};

static Station *stations;

void stationsInit()
{
//    eeErasePages(EE_PAGE_FM, 1);
//    eeWritePage(EE_PAGE_FM, st, EE_PAGE_SIZE);
    stations = (Station *)(eeGetPageAddr(EE_PAGE_FM));
//    stations = st;
}

void stationSeek(int8_t direction)
{
    const uint16_t freq = tunerGet()->status.freq;

    for (int8_t i = 0; i < STATION_COUNT; i++) {
        uint16_t stFreq = stations[i].freq;
        if (stFreq == 0 || stFreq == 0xFFFF) {
            break;
        }

        if (direction >= 0) {
            if (stFreq <= freq) {
                continue;
            }
            stationZap(i);
        } else {
            if (stFreq < freq) {
                continue;
            }
            stationZap(i - 1);
        }
        break;
    }
}

int8_t stationGetNum(uint16_t freq)
{
    for (int8_t i = 0; i < STATION_COUNT; i++) {
        uint16_t stFreq = stations[i].freq;
        if (stFreq == 0 || stFreq == 0xFFFF) {
            break;
        }
        if (stFreq == freq) {
            return i;
        }
    }

    return STATION_NOT_FOUND;
}

char *stationGetName(int8_t num)
{
    if (num >= 0 && num < STATION_COUNT) {
        return stations[num].name;
    }

    return "";
}

void stationZap(int8_t num)
{
    if (num < 0) {
        return;
    }

    tunerSetFreq(stations[num].freq);
}

void stationStore(uint16_t freq)
{

}

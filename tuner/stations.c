#include "stations.h"

#include "tuner.h"

static Station stations[STATION_COUNT] = {
    [0] = {8750},
    [1] = {9240},
    [2] = {9370},
    [3] = {9410},
    [4] = {9620},
    [5] = {9740},
    [6] = {9800},
    [7] = {9840},
    [8] = {9890},
    [9] = {9950},
    [10] = {10040},
    [11] = {10120},
    [12] = {10170},
    [13] = {10210},
    [14] = {10250},
    [15] = {10290},
    [16] = {10370},
    [17] = {10460},
    [18] = {10510},
    [19] = {10570},
    [20] = {10620},
    [21] = {10710},
    [22] = {10790},
};

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

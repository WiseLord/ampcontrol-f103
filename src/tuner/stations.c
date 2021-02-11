#include "stations.h"

#include <stdlib.h>
#include <string.h>

#include "eemul.h"
#include "settings.h"
#include "tuner.h"

#define STATION_COUNT       (int8_t)(EE_PAGE_SIZE / sizeof(Station))
#define EE_PAGE_FM          (EE_PAGE_COUNT - EE_PAGE_STEP * 2 - 1)

static Station *stFlash;
uint16_t stFav[STATION_FAV_COUNT];

void stationsInit()
{
    stFlash = (Station *)(eeGetPageAddr(EE_PAGE_FM));
}

void stationSeek(int8_t direction)
{
    const uint16_t freq = tunerGet()->status.freq;
    uint16_t lastFreq = freq;

    for (int8_t i = 0; i < STATION_COUNT; i++) {
        uint16_t stFreq = stFlash[i].freq;
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
                lastFreq = stFreq;
                continue;
            }
            stationZap(i - 1);
        }
        break;
    }

    if (lastFreq != freq) {
        tunerSetFreq(lastFreq);
    }
}

int8_t stationGetCount(void)
{
    int8_t ret = 0;

    for (int8_t i = 0; i < STATION_COUNT; i++) {
        if (stFlash[i].freq == 0x0000 || stFlash[i].freq == 0xFFFF) {
            break;
        }
        ret++;
    }

    return ret;
}

int8_t stationGetNum(uint16_t freq)
{
    for (int8_t i = 0; i < STATION_COUNT; i++) {
        uint16_t stFreq = stFlash[i].freq;
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
        return stFlash[num].name;
    }

    return "";
}

void stationZap(int8_t num)
{
    if (num < 0 || num >= STATION_COUNT) {
        return;
    }

    if (stFlash[num].freq != EE_EMPTY) {
        tunerSetFreq(stFlash[num].freq);
    }
}

void stationStore(uint16_t freq, char *name)
{
    uint16_t idx = 0;
    uint16_t num = 0;
    bool saved = false;

    Station *stRam = malloc(sizeof(Station) * STATION_COUNT);

    while (num < STATION_COUNT) {
        uint16_t stFreq = stFlash[idx].freq;

        if (stFreq == 0x0000 || stFreq == 0xFFFF) {
            if (!saved) {
                // Store requested freq
                stRam[num].freq = freq;
                strncpy(stRam[num].name, name, STATION_NAME_MAX_LEN);
                saved = true;
                num++;
            }
            break;
        }

        if (stFreq < freq) {
            // Copy old station
            memcpy(&stRam[num], &stFlash[idx], sizeof(Station));
            num++;
        } else if (stFreq == freq) {
            // Replace old station with new one
            stRam[num].freq = freq;
            strncpy(stRam[num].name, name, STATION_NAME_MAX_LEN);
            saved = true;
            num++;
        } else {
            if (!saved) {
                // Insert new station
                stRam[num].freq = freq;
                strncpy(stRam[num].name, name, STATION_NAME_MAX_LEN);
                saved = true;
                num++;
                continue;
            }
            // Copy old station
            memcpy(&stRam[num], &stFlash[idx], sizeof(Station));
            num++;
        }
        idx++;
    }

    // Save array in RAM to Flash
    eeErasePages(EE_PAGE_FM, 1);
    eeWritePage(EE_PAGE_FM, stRam, (uint16_t)((num) * sizeof(Station)));

    free(stRam);
}

void stationRemove(uint16_t freq)
{
    uint16_t idx = 0;
    uint16_t num = 0;
    bool deleted = false;

    Station *stRam = malloc(sizeof(Station) * STATION_COUNT);

    while (idx < STATION_COUNT) {
        uint16_t stFreq = stFlash[idx].freq;

        if (stFreq == 0x0000 || stFreq == 0xFFFF) {
            if (!deleted) {
                // Not found station to delete
                free(stRam);
                return;
            }
            break;
        }

        if (stFreq == freq) {
            deleted = true;
        } else {
            // Copy old station
            memcpy(&stRam[num], &stFlash[idx], sizeof(Station));
            num++;
        }
        idx++;
    }

    // Save array in RAM to Flash
    eeErasePages(EE_PAGE_FM, 1);
    eeWritePage(EE_PAGE_FM, stRam, (uint16_t)((num) * sizeof(Station)));

    free(stRam);
}

void stationStoreRemove(uint16_t freq, char *name)
{
    if (stationGetNum(freq) < 0) {
        stationStore(freq, name);
    } else {
        stationRemove(freq);
    }
}

void stationFavInit(void)
{
    for (uint8_t i = 0; i < STATION_FAV_COUNT; i++) {
        uint16_t raw = (uint16_t)settingsRead(PARAM_TUNER_FAV_0 + i, 0);
        if (raw != EE_NOT_FOUND) {
            stFav[i] = raw;
        }
    }
}

void stationFavZap(int8_t num)
{
    if (num < 0 || num >= STATION_FAV_COUNT) {
        return;
    }

    if (stFav[num] != 0) {
        tunerSetFreq(stFav[num]);
    }
}

void stationFavStoreRemove(int8_t num)
{
    if (num < 0 || num >= STATION_FAV_COUNT) {
        return;
    }

    uint16_t freq = tunerGet()->status.freq;

    for (int8_t i = 0; i < STATION_FAV_COUNT; i++) {
        if (stFav[i] == freq) {
            stFav[i] = 0;
            settingsStore(PARAM_TUNER_FAV_0 + i, stFav[i]);
        } else if (i == num) {
            stFav[i] = freq;
            settingsStore(PARAM_TUNER_FAV_0 + i, stFav[i]);
        }
    }
}

uint16_t stationFavGetMask(uint16_t freq)
{
    uint16_t mask = 0x0000;

    for (uint8_t i = 0; i < STATION_FAV_COUNT; i++) {
        if (freq == stFav[i]) {
            mask |= (1 << i);
        }
    }

    return mask;
}

#ifndef STATIONS_H
#define STATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define STATION_NOT_FOUND       -1

#include <stdint.h>

#define STATION_NAME_MAX_LEN    30
#define STATION_NAME_MAX_SYM    18

#define STATION_FAV_COUNT       10

typedef struct {
    uint16_t freq;
    char name[STATION_NAME_MAX_LEN];
} Station;

void stationsInit(void);

void stationSeek(int8_t direction);

int8_t stationGetCount(void);

int8_t stationGetNum(uint16_t freq);
char *stationGetName(int8_t num);

void stationZap(int8_t num);
void stationStore(uint16_t freq, char *name);
void stationRemove(uint16_t freq);

void stationFavInit(void);
void stationFavZap(int8_t num);
void stationFavStoreRemove(int8_t num);
uint16_t stationFavGetMask(uint16_t freq);

#ifdef __cplusplus
}
#endif

#endif // STATIONS_H

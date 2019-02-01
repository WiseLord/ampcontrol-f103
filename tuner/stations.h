#ifndef STATIONS_H
#define STATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define STATION_NOT_FOUND       -1
#define STATION_COUNT           32

#include <stdint.h>

typedef struct {
    uint16_t freq;
    char name[30];
} Station;

void stationsInit(void);

void stationSeek(int8_t direction);

int8_t stationGetNum(uint16_t freq);
char *stationGetName(int8_t num);

void stationZap(int8_t num);
void stationStore(uint16_t freq);

#ifdef __cplusplus
}
#endif

#endif // STATIONS_H

#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>

#include "rtc.h"

#define MIN_BRIGHTNESS          GD_MIN_BRIGHTNESS
#define MAX_BRIGHTNESS          GD_MAX_BRIGHTNESS

#define STR_BUFSIZE             20

typedef struct {
    const char *label;
    int16_t value;
    int16_t min;
    int16_t max;
    uint8_t icon;
} DispParam;

void displayInit();

int8_t displayGetBrightness(uint8_t mode);
void displayChangeBrighness(uint8_t mode, int8_t diff);

void displayShowTime(RTC_type *rtc, char *wday);
void displayShowParam(DispParam *dp);
void displayShowSpectrum(uint16_t *dataL, uint16_t *dataR);

#endif // DISPLAY_H

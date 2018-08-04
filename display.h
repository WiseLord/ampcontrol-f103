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

typedef struct {
    uint16_t width;
    uint16_t height;
    void (*writeChar)(uint8_t code);
    void (*writeString)(char *string);
    void (*showTime)(RTC_type *rtc, char *wday);
    void (*showParam)(DispParam *dp);
    void (*showSpectrum)(uint8_t *dataL, uint8_t *dataR);
} Display;

void displayInit();

int8_t displayGetBrightness(uint8_t mode);
void displayChangeBrighness(uint8_t mode, int8_t diff);

void displayWriteNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix);

void displayShowTime(RTC_type *rtc, char *wday);
void displayShowParam(DispParam *dp);
void displayShowSpectrum(uint8_t *dataL, uint8_t *dataR);

#endif // DISPLAY_H

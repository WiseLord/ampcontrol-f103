#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>

#include "display/glcd.h"

void displayInit();
void displayClear();
void displayUpdateIRQ();

int8_t displayGetBrightness(uint8_t mode);
void displayChangeBrighness(uint8_t mode, int8_t diff);

uint8_t displayReadBus(void);

void displayShowTime(RTC_type *rtc, char *wday);
void displayShowParam(DispParam *dp);
void displayShowSpectrum(uint8_t *dataL, uint8_t *dataR);

#endif // DISPLAY_H

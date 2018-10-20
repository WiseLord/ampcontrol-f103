#ifndef CANVAS_H
#define CANVAS_H

#include <inttypes.h>

#include "../display/glcd.h"
#include "../rtc.h"
#include "../spectrum.h"
#include "../tuner/tuner.h"

typedef struct {
    const char *label;
    int16_t value;
    int16_t min;
    int16_t max;
    uint8_t step;
    uint8_t icon;
} DispParam;

typedef struct {
    Tuner *tuner;
} DispTuner;

typedef struct {
    Glcd *glcd;

    void (*showTime)(RTC_type *rtc, char *wday);
    void (*showParam)(DispParam *dp);
    void (*showSpectrum)(SpectrumData *spData);
    void (*showTuner)(DispTuner *dt);

    uint16_t color;
} Canvas;

void canvasInit(Canvas **canvas);

#endif // CANVAS_H

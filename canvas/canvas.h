#ifndef CANVAS_H
#define CANVAS_H

#include <stdint.h>
#include <stddef.h>

#include "../display/glcd.h"
#include "../rtc.h"
#include "../spectrum.h"
#include "../tuner/tuner.h"
#include "../menu.h"

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
    uint16_t width;
    uint16_t height;

    void (*showTime)(RTC_type *rtc, char *wday);
    void (*showParam)(DispParam *dp);
    void (*showSpectrum)(SpectrumData *spData);
    void (*showTuner)(DispTuner *dt);
    void (*showMenu)(void);

    uint16_t color;
} Canvas;

void canvasInit(Canvas **canvas);

void canvasShowMenu(const tFont *fontHeader, const tFont *fontItem);

#endif // CANVAS_H

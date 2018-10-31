#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "../display/glcd.h"
#include "../tr/labels.h"
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
    int16_t sc;
    uint8_t sw;
    uint16_t pos;
    uint8_t half;
    uint8_t middle;
} CanvasBar;

typedef struct {
    const tFont *hmsFont;
    const tFont *dmyFont;
    const tFont *wdFont;

    uint8_t hmsY;
    uint8_t dmyY;
    uint8_t wdY;
} CanvasTime;

typedef struct {
    Glcd *glcd;
    uint16_t width;
    uint16_t height;

    void (*showTime)(bool clear, RTC_type *rtc);
    void (*showParam)(DispParam *dp);
    void (*showSpectrum)(bool clear, SpectrumData *spData);
    void (*showTuner)(DispTuner *dt);
    void (*showMenu)(void);

    uint16_t color;
} Canvas;

// Canvas variants
void gc160x128Init(Canvas **driver);
void gc176x132Init(Canvas **driver);
void gc220x176Init(Canvas **driver);
void gc320x240Init(Canvas **driver);
void gc400x240Init(Canvas **driver);
void gc480x320Init(Canvas **driver);
void gm128x64Init(Canvas **driver);
#ifdef EMUL_DISP
void emulCanvasInit(Canvas **driver);
#endif

void canvasInit(Canvas **canvas);

void canvasDrawBar(int16_t value, int16_t min, int16_t max, const CanvasBar *bar);

void canvasShowTime(bool clear, const CanvasTime *ct, RTC_type *rtc);
void canvasShowSpectrum(bool clear, SpectrumData *spData, uint8_t step, uint8_t oft, uint8_t width);
void canvasShowTuner(DispTuner *dt, const tFont *fmFont, const CanvasBar *bar);
void canvasShowMenu(const tFont *fontHeader, const tFont *fontItem);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H

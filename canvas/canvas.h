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
    struct {
        const tFont *hmsFont;       // Font to draw hours/minutes/seconds
        const tFont *dmyFont;       // Font to draw day/month/year
        const tFont *wdFont;        // Font to draw weekday
        uint8_t hmsY;               // Y position of hours/minutes/seconds
        uint8_t dmyY;               // Y position of day/month/year
        uint8_t wdY;                // Y position of weekday
    } time;
    struct {
        int16_t sc;
        uint8_t sw;
        uint16_t pos;
        uint8_t half;
        uint8_t middle;
    } bar;
    struct {
        const tFont *headFont;      // Font to draw menu header
        const tFont *menuFont;      // Foft to draw menu item
        const uint8_t itemCnt;      // Number of items can be shown
    } menu;
} CanvasParam;

typedef struct {
    Glcd *glcd;
    uint16_t width;
    uint16_t height;

    void (*showParam)(DispParam *dp);
    void (*showSpectrum)(bool clear, SpectrumData *spData);
    void (*showTuner)(DispTuner *dt);

    uint16_t color;

    const CanvasParam *par;
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

void canvasDrawBar(int16_t value, int16_t min, int16_t max);

void canvasShowTime(bool clear, RTC_type *rtc);
void canvasShowSpectrum(bool clear, SpectrumData *spData, uint8_t step, uint8_t oft, uint8_t width);
void canvasShowTuner(DispTuner *dt, const tFont *fmFont);
void canvasShowMenu(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H

#ifndef SPECTRUM_H
#define SPECTRUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t SpChan;
enum {
    SP_CHAN_LEFT = 0,
    SP_CHAN_RIGHT,
    SP_CHAN_BOTH,

    SP_CHAN_END = SP_CHAN_BOTH
};

typedef uint8_t SpMode;
enum {
    SP_MODE_STEREO = 0,
    SP_MODE_MIXED,
    SP_MODE_WATERFALL,

    SP_MODE_END
};

#define SPECTRUM_SIZE   128
#define N_DB            256

typedef struct {
    uint8_t raw[SPECTRUM_SIZE];
    uint8_t avg;
    uint8_t max;
} SpData;

typedef struct {
    SpData data[SP_CHAN_END];
    SpMode mode;
    bool peaks;
    bool grad;
    int16_t wtfX;  // waterfall X position
    bool ready;
    bool redraw;
} Spectrum;

void spInit(void);
Spectrum *spGet(void);

void spGetADC(Spectrum *sp);

void spConvertADC(void);

#ifdef __cplusplus
}
#endif

#endif // SPECTRUM_H

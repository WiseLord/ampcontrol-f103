#include "screen.h"

#include "display/display.h"
#include "actions.h"
#include "fft.h"
#include "spectrum.h"

static GlcdDriver *glcd;
static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;

typedef struct {
    uint8_t data[FFT_SIZE / 2];
    uint8_t fall[FFT_SIZE / 2];
    uint8_t show[FFT_SIZE / 2];
} SpectrumData;

static SpectrumData spLeft;
static SpectrumData spRight;

// TODO: Read from backup memory
static int8_t brStby = 1;
static int8_t brWork = 32;

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_BRIGNTNESS,

    LABEL_END
} TxtLabel;

const char *txtLabels[LABEL_END] = {
    [LABEL_SUNDAY]          = "  SUNDAY  ",
    [LABEL_MONDAY]          = "  MONDAY  ",
    [LABEL_TUESDAY]         = " TUESDAY  ",
    [LABEL_WEDNESDAY]       = "WEDNESDAY ",
    [LABEL_THURSDAY]        = " THURSDAY ",
    [LABEL_FRIDAY]          = "  FRIDAY  ",
    [LABEL_SATURDAY]        = " SATURDAY ",

    [LABEL_BRIGNTNESS]      = "Brightness",
};

void screenInit(void)
{
    displayInit(&glcd);
    screenClear();
    displaySetBrightness(brStby);
}

void screenClear(void)
{
    glcdClear();
}

void screenUpdate(void)
{
    glcdUpdate();
}

void screenSet(Screen value)
{
    screen = value;
}

Screen screenGet()
{
    return screen;
}

void screenSetDefault(Screen value)
{
    screenDefault = value;
}

Screen screenGetDefault(void)
{
    return screenDefault;
}

int8_t screenGetBrightness(uint8_t mode)
{
    if (mode == ACTION_BR_WORK)
        return brWork;
    else
        return brStby;
}

void screenSetBrightness(uint8_t mode, int8_t value)
{
    if (mode == ACTION_BR_WORK)
        brWork = value;
    else
        brStby = value;

    displaySetBrightness(value);
}

void screenChangeBrighness(uint8_t mode, int8_t diff)
{
    int8_t br = screenGetBrightness(mode);

    br += diff;

    if (br > GLCD_MAX_BRIGHTNESS)
        br = GLCD_MAX_BRIGHTNESS;
    if (br < GLCD_MIN_BRIGHTNESS)
        br = GLCD_MIN_BRIGHTNESS;

    screenSetBrightness(mode, br);
}

static void improveSpectrum (SpectrumData *sd)
{
    for (uint8_t i = 0; i < FFT_SIZE / 2; i++) {
        if (sd->data[i] < sd->show[i]) {
            if (sd->show[i] >= sd->fall[i]) {
                sd->show[i] -= sd->fall[i];
                sd->fall[i]++;
            } else {
                sd->show[i] = 0;
            }
        }

        if (sd->data[i] > sd->show[i]) {
            sd->show[i] = sd->data[i];
            sd->fall[i] = 0;
        }
    }
}


void screenShow(void)
{
    static Screen screenPrev = SCREEN_STANDBY;
    Screen screen = screenGet();

    // Clear display if screen mode has changed (but not standby/time screens)
    if (screen != screenPrev) {
        if (screen > SCREEN_TIME || screenPrev > SCREEN_TIME)
            screenClear();
    }

    switch (screen) {
    case SCREEN_STANDBY:
        screenTime();
        break;
    case SCREEN_TIME:
        screenTime();
        break;
    case SCREEN_SPECTRUM:
        screenSpectrum();
        break;
    case SCREEN_BRIGHTNESS:
        screenBrightness();
        break;
    default:
        break;
    }

    // Save current screen as previous
    screenPrev = screen;

    screenUpdate();
}

void screenTime(void)
{
    RTC_type rtc;
    rtc.etm = rtcGetMode();

    rtcGetTime(&rtc);

    if (glcd->canvas->showTime) {
        glcd->canvas->showTime(&rtc, (char *)txtLabels[LABEL_SUNDAY + rtc.wday]);
    }
}

void screenSpectrum(void)
{
    spGetADC(spLeft.data, spRight.data);

    improveSpectrum(&spLeft);
    improveSpectrum(&spRight);

    if (glcd->canvas->showSpectrum) {
        glcd->canvas->showSpectrum(spLeft.show, spRight.show);
    }
}

void screenBrightness()
{
    DispParam dp;

    dp.label = txtLabels[LABEL_BRIGNTNESS];
    dp.value = screenGetBrightness(ACTION_BR_WORK);
    dp.min = GLCD_MIN_BRIGHTNESS;
    dp.max = GLCD_MAX_BRIGHTNESS;
    dp.icon = ICON24_BRIGHTNESS;

    if (glcd->canvas->showParam) {
        glcd->canvas->showParam(&dp);
    }
}

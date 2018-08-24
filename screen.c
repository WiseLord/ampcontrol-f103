#include "screen.h"

#include "display/icons.h"
#include "actions.h"
#include "fft.h"
#include "spectrum.h"

#if defined (_KS0108)
#include "display/ks0108.h"
#elif defined (_SSD1306)
#include "display/ssd1306.h"
#elif defined (_LS020)
#include "display/ls020.h"
#elif defined (_LPH9157)
#include "display/lph9157.h"
#elif defined (_SSD1286A)
#include "display/ssd1286a.h"
#elif defined (_ILI9320)
#include "display/ili9320.h"
#elif defined (_ILI9341)
#include "display/ili9341.h"
#elif defined (_S6D0139)
#include "display/s6d0139.h"
#elif defined (_SPFD5408)
#include "display/spfd5408.h"
#else
#error "Unsupported display driver"
#endif

static GlcdDriver *glcd;
static Screen screen = SCREEN_STANDBY;

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
#if defined (_KS0108)
    ks0108Init(&glcd);
#elif defined (_SSD1306)
    ssd1306Init(&glcd);
#elif defined (_LS020)
    ls020Init(&glcd);
#elif defined (_LPH9157)
    lph9157Init(&glcd);
#elif defined (_SSD1286A)
    ssd1286aInit(&glcd);
#elif defined (_ILI9320)
    ili9320Init(&glcd);
#elif defined (_ILI9341)
    ili9341Init(&glcd);
#elif defined (_S6D0139)
    s6d0139Init(&glcd);
#elif defined (_SPFD5408)
    spfd5408Init(&glcd);
#else
#error "Unsupported display driver"
#endif
    screenClear();
    glcdSetBrightness(brStby);
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

int8_t screenGetBrightness(uint8_t mode)
{
    if (mode == AMODE_BRIGHTNESS_WORK)
        return brWork;
    else
        return brStby;
}

void screenSetBrightness(uint8_t mode, int8_t value)
{
    if (mode == AMODE_BRIGHTNESS_WORK)
        brWork = value;
    else
        brStby = value;

    glcdSetBrightness(value);
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

void screenTime(RtcMode etm)
{
    RTC_type rtc;
    rtc.etm = etm;

    rtcGetTime(&rtc);

    if (glcd->canvas->showTime) {
        glcd->canvas->showTime(&rtc, (char *)txtLabels[LABEL_SUNDAY + rtc.wday]);
    }
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
    dp.value = screenGetBrightness(AMODE_BRIGHTNESS_WORK);
    dp.min = GLCD_MIN_BRIGHTNESS;
    dp.max = GLCD_MAX_BRIGHTNESS;
    dp.icon = ICON24_BRIGHTNESS;

    if (glcd->canvas->showParam) {
        glcd->canvas->showParam(&dp);
    }
}

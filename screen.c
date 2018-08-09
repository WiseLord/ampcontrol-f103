#include "screen.h"

#include "display/icons.h"
#include "actions.h"
#include "display.h"
#include "fft.h"
#include "spectrum.h"

static Screen screen = SCREEN_STANDBY;

static uint8_t spLeft[FFT_SIZE / 2];
static uint8_t spRight[FFT_SIZE / 2];

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

void screenSet(Screen value)
{
    screen = value;
}

Screen screenGet()
{
    return screen;
}

void screenClear(void)
{
    displayClear();
}

void screenTime(RtcMode etm)
{
    RTC_type rtc;
    rtc.etm = etm;

    rtcGetTime(&rtc);

    displayShowTime(&rtc, (char *)txtLabels[LABEL_SUNDAY + rtc.wday]);
}

void screenSpectrum(uint8_t speed)
{
    spGetADC(spLeft, spRight, speed);

    displayShowSpectrum(spLeft, spRight);
}

void screenBrightness()
{
    DispParam dp;

    dp.label = txtLabels[LABEL_BRIGNTNESS];
    dp.value = displayGetBrightness(AMODE_BRIGHTNESS_WORK);
    // TODO: Use param from driver/layout
    dp.min = 0;
    dp.max = 32;
    dp.icon = ICON24_BRIGHTNESS;

    displayShowParam(&dp);
}

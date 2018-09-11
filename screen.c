#include "screen.h"

#include "display/display.h"
#include "actions.h"
#include "eemul.h"
#include "fft.h"
#include "spectrum.h"

static GlcdDriver *glcd;
static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;
static ScreenParam scrPar;

static SpectrumData spData[SP_CHAN_END];

// TODO: Read from backup memory
static int8_t brStby;
static int8_t brWork;

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_BRIGNTNESS,

    LABEL_VOLUME,
    LABEL_BASS,
    LABEL_MIDDLE,
    LABEL_TREBLE,
    LABEL_PREAMP,
    LABEL_FRONTREAR,
    LABEL_BALANCE,
    LABEL_CENTER,
    LABEL_SUBWOOFER,
    LABEL_GAIN0,
    LABEL_GAIN1,
    LABEL_GAIN2,
    LABEL_GAIN3,
    LABEL_GAIN4,

    LABEL_END
} TxtLabel;

const char *txtLabels[LABEL_END] = {
    [LABEL_SUNDAY]          = "НЯДЗЕЛЯ",
    [LABEL_MONDAY]          = "ПАНЯДЗЕЛАК",
    [LABEL_TUESDAY]         = "АЎТОРАК",
    [LABEL_WEDNESDAY]       = "СЕРЕДА",
    [LABEL_THURSDAY]        = "ЧАЦВЕР",
    [LABEL_FRIDAY]          = "ПЯТНІЦА",
    [LABEL_SATURDAY]        = "СУБОТА",

    [LABEL_BRIGNTNESS]      = "Яркасць",

    [LABEL_VOLUME]          = "Гучнасць",
    [LABEL_BASS]            = "Тэмбр НЧ",
    [LABEL_MIDDLE]          = "Тэмбр СЧ",
    [LABEL_TREBLE]          = "Тэмбр ВЧ",
    [LABEL_PREAMP]          = "Пасіленне",
    [LABEL_FRONTREAR]       = "Фронт/тыл",
    [LABEL_BALANCE]         = "Баланс",
    [LABEL_CENTER]          = "Цэнтр",
    [LABEL_SUBWOOFER]       = "Сабвуфер",
    [LABEL_GAIN0]           = "FM цюнэр",
    [LABEL_GAIN1]           = "Камп'ютар",
    [LABEL_GAIN2]           = "Тэлевізар",
    [LABEL_GAIN3]           = "Bluetooth",
    [LABEL_GAIN4]           = "DVD-плэер",
};

static void screenReadSettings(void)
{
    uint16_t eeData;

    eeData = eeRead(EE_BRIGHTNESS_STBY);
    brStby = (eeData > GLCD_MAX_BRIGHTNESS ? 2 : (int8_t)(eeData));

    eeData = eeRead(EE_BRIGHTNESS_WORK);
    brWork = (eeData > GLCD_MAX_BRIGHTNESS ? GLCD_MAX_BRIGHTNESS : (int8_t)(eeData));
}

void screenSaveSettings(void)
{
    eeUpdate(EE_BRIGHTNESS_STBY, brStby);
    eeUpdate(EE_BRIGHTNESS_WORK, brWork);
}

void screenInit(void)
{
    displayInit(&glcd);
    screenClear();

    screenReadSettings();

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

void screenSetParam(ScreenParam param)
{
    scrPar = param;
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

static void improveSpectrum(SpectrumData *sd)
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

static void screenTime(void)
{
    RTC_type rtc;
    rtc.etm = rtcGetMode();

    rtcGetTime(&rtc);

    if (glcd->canvas->showTime) {
        glcd->canvas->showTime(&rtc, (char *)txtLabels[LABEL_SUNDAY + rtc.wday]);
    }
}

static void screenSpectrum(void)
{
    spGetADC(spData[SP_CHAN_LEFT].data, spData[SP_CHAN_RIGHT].data);

    improveSpectrum(&spData[SP_CHAN_LEFT]);
    improveSpectrum(&spData[SP_CHAN_RIGHT]);

    if (glcd->canvas->showSpectrum) {
        glcd->canvas->showSpectrum(spData);
    }
}

static void screenBrightness()
{
    DispParam dp;

    dp.label = txtLabels[LABEL_BRIGNTNESS];
    dp.value = screenGetBrightness(ACTION_BR_WORK);
    dp.min = GLCD_MIN_BRIGHTNESS;
    dp.max = GLCD_MAX_BRIGHTNESS;
    dp.step = 1 * 8;
    dp.icon = ICON24_BRIGHTNESS;

    if (glcd->canvas->showParam) {
        glcd->canvas->showParam(&dp);
    }
}

static void screenAudioParam()
{
    AudioProc *aProc = audioProcGet();
    AudioParam aPar = scrPar.audio;

    if (aPar >= AUDIO_PARAM_END)
        aPar = AUDIO_PARAM_VOLUME;

    DispParam dp;
    dp.label = txtLabels[LABEL_VOLUME + aPar];
    dp.value = aProc->item[aPar].value;
    dp.min = aProc->item[aPar].grid->min;
    dp.max = aProc->item[aPar].grid->max;
    dp.step = aProc->item[aPar].grid->step;
    dp.icon = ICON24_VOLUME + aPar;

    if (glcd->canvas->showParam) {
        glcd->canvas->showParam(&dp);
    }
}

static void screenTest()
{
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(LCD_COLOR_WHITE);

    uint16_t *value;

    for (uint8_t i = 0; i < 16; i++) {
        glcdSetXY((i % 2) ? 28 : 0, 12 * (i / 2));
        value = (uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_0 + 2 * i);
        glcdWriteNum(*value, 4, '0', 16);
    }

    for (uint8_t i = 0; i < 16; i++) {
        glcdSetXY((i % 2) ? 92 : 64, 12 * (i / 2));
        value = (uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_1 + 2 * i);
        glcdWriteNum(*value, 4, '0', 16);
    }
}

void screenShow(void)
{
    static Screen screenPrev = SCREEN_STANDBY;
    static ScreenParam scrParPrev;

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
    case SCREEN_AUDIO_INPUT:
    case SCREEN_AUDIO_PARAM:
        if (scrPar.audio != scrParPrev.audio)
            screenClear();
        screenAudioParam();
        break;

    case SCREEN_TEST:
        screenTest();
        break;
    default:
        break;
    }

    // Save current screen as previous
    screenPrev = screen;
    scrParPrev = scrPar;

    screenUpdate();
}

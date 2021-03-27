#ifndef TR_H
#define TR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "amp.h"
#include "audio/audio.h"
#include "gui/palette.h"
#include "menu.h"
#include "rtc.h"
#include "spectrum.h"
#include "tuner/tuner.h"

#define GENERATE_MENU_RC_TEXT(CMD)  [LABEL_MENU + MENU_RC_ ## CMD] = # CMD,
#define GENERATE_AUDIO_IC_TEXT(IC)  [LABEL_AUDIO_IC + AUDIO_IC_ ## IC] = # IC,
#define GENERATE_TUNER_IC_TEXT(IC)  [LABEL_TUNER_IC + TUNER_IC_ ## IC] = # IC,

typedef uint8_t Lang;
enum {
    LANG_EN,
    LANG_BY,
    LANG_RU,
    LANG_UA,
    LANG_TR,

    LANG_END,

    LANG_DEFAULT = LANG_EN,
};

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_VOLUME,
    LABEL_BASS,
    LABEL_MIDDLE,
    LABEL_TREBLE,
    LABEL_BALANCE,
    LABEL_FRONTREAR,
    LABEL_CENTER,
    LABEL_SUBWOOFER,
    LABEL_PREAMP,
    LABEL_LOUDNESS,

    LABEL_BASS_FREQ,
    LABEL_BASS_QUAL,
    LABEL_MIDDLE_KFREQ,
    LABEL_MIDDLE_QUAL,
    LABEL_TREBLE_KFREQ,
    LABEL_SUB_CUT_FREQ,
    LABEL_LOUD_PEAK_FREQ,

    LABEL_IN_TUNER,
    LABEL_IN_PC,
    LABEL_IN_TV,
    LABEL_IN_BLUETOOTH,
    LABEL_IN_DVD,
    LABEL_IN_USB,
    LABEL_IN_MICROPHONE,
    LABEL_IN_GUITAR,
    LABEL_IN_TURNTABLES,
    LABEL_IN_SDCARD,
    LABEL_IN_PROJECTOR,
    LABEL_IN_SATELLITE,
    LABEL_IN_MPD,
    LABEL_IN_KARADIO,

    LABEL_BOOL_OFF,
    LABEL_BOOL_ON,

    LABEL_DB,
    LABEL_HZ,
    LABEL_K,

    LABEL_MPD_WAIT,

    LABEL_TUNER_FM_STATION_NAME,

    LABEL_TUNER_IC,
    LABEL_TUNER_IC_END = LABEL_TUNER_IC + (TUNER_IC_END - TUNER_IC_NO),

    LABEL_TUNER_BAND = LABEL_TUNER_IC_END,
    LABEL_TUNER_BAND_END = LABEL_TUNER_BAND + (TUNER_BAND_END - TUNER_BAND_FM_US_EUROPE),

    LABEL_TUNER_STEP = LABEL_TUNER_BAND_END,
    LABEL_TUNER_STEP_END = LABEL_TUNER_STEP + (TUNER_STEP_END - TUNER_STEP_50K),

    LABEL_TUNER_DEEMPH = LABEL_TUNER_STEP_END,
    LABEL_TUNER_DEEMPH_END = LABEL_TUNER_DEEMPH + (TUNER_DEEMPH_END - TUNER_DEEMPH_50u),

    LABEL_SPECTRUM_MODE = LABEL_TUNER_DEEMPH_END,
    LABEL_SPECTRUM_MODE_END = LABEL_SPECTRUM_MODE + (SP_MODE_END - SP_MODE_STEREO),

    LABEL_PAL_MODE = LABEL_SPECTRUM_MODE_END,
    LABEL_PAL_MODE_END = LABEL_PAL_MODE + (PAL_END - PAL_DEFAULT),

    LABEL_AUDIO_IC = LABEL_PAL_MODE_END,
    LABEL_AUDIO_IC_END = LABEL_AUDIO_IC + (AUDIO_IC_END - AUDIO_IC_NO),

    LABEL_AUDIO_MODE = LABEL_AUDIO_IC_END,
    LABEL_AUDIO_MODE_END = LABEL_AUDIO_MODE + (AUDIO_MODE_END - AUDIO_MODE_2_0),

    LABEL_ALARM_DAY = LABEL_AUDIO_MODE_END,
    LABEL_ALARM_DAY_END = LABEL_ALARM_DAY + (ALARM_DAY_END - ALARM_DAY_OFF),

    // Menu
    LABEL_MENU = LABEL_ALARM_DAY_END,
    LABEL_MENU_END = LABEL_MENU + (MENU_END - MENU_NULL),

    LABEL_END = LABEL_MENU_END,
} Label;

extern const char *const labels_en[LABEL_END];
extern const char *const labels_by[LABEL_END];
extern const char *const labels_fr[LABEL_END];
extern const char *const labels_ru[LABEL_END];
extern const char *const labels_tr[LABEL_END];
extern const char *const labels_ua[LABEL_END];
extern const char *const labels_ro[LABEL_END];

void labelsSetLang(Lang value);
Lang labelsGetLang(void);
const char *labelsGetLangName(Lang value);

const char *labelsGetDefault(Label value);
const char *labelsGet(Label value);

void labelsInit(void);

#ifdef __cplusplus
}
#endif

#endif // TR_H

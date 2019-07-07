#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "cmd.h"

#define GENERATE_MENU_RC(CMD)    MENU_RC_ ## CMD,

typedef uint8_t MenuIdx;
enum {
    MENU_NULL = 0,

    MENU_SETUP,

    MENU_SETUP_SYSTEM,
    MENU_SETUP_AUDIO,
    MENU_SETUP_TUNER,
    MENU_SETUP_SPECTRUM,
    MENU_SETUP_DISPLAY,
    MENU_SETUP_ALARM,
    MENU_SETUP_RC,

    MENU_SYSTEM_LANG,
    MENU_SYSTEM_MUTESTBY,
    MENU_SYSTEM_ENC_RES,
    MENU_SYSTEM_SIL_TIM,
    MENU_SYSTEM_RTC_CORR,

    MENU_AUDIO_IC,
    MENU_AUDIO_IN_0,
    MENU_AUDIO_IN_1,
    MENU_AUDIO_IN_2,
    MENU_AUDIO_IN_3,
    MENU_AUDIO_IN_4,
    MENU_AUDIO_IN_5,
    MENU_AUDIO_IN_6,
    MENU_AUDIO_IN_7,

    MENU_TUNER_IC,
    MENU_TUNER_BAND,
    MENU_TUNER_STEP,
    MENU_TUNER_DEEMPH,
    MENU_TUNER_MODE,
    MENU_TUNER_FMONO,
    MENU_TUNER_RDS,
    MENU_TUNER_BASS,
    MENU_TUNER_VOLUME,

    MENU_ALARM_HOUR,
    MENU_ALARM_MINUTE,
    MENU_ALARM_DAYS,

    MENU_SPECTURM_MODE,
    MENU_SPECTRUM_PEAKS,

    MENU_DISPLAY_BR_STBY,
    MENU_DISPLAY_BR_WORK,
    MENU_DISPLAY_ROTATE,
    MENU_DISPLAY_PALETTE,

    FOREACH_CMD(GENERATE_MENU_RC)

    MENU_END,

    // Some aliases
    MENU_AUDIO_IN = MENU_AUDIO_IN_0,
    MENU_RC_DIG = MENU_RC_DIG_0,
};

#define MENU_MAX_LEN    (MENU_END - MENU_RC_STBY_SWITCH + 1)

typedef uint8_t MenuType;
enum {
    MENU_TYPE_PARENT,
    MENU_TYPE_BOOL,
    MENU_TYPE_NUMBER,
    MENU_TYPE_ENUM,
    MENU_TYPE_RC,

    MENU_TYPE_END
};

typedef struct {
    MenuIdx parent;
    MenuIdx active;
    uint8_t list[MENU_MAX_LEN];
    uint8_t listSize;       // Total number of items in current menu
    uint8_t selected;
    int16_t value;

    uint8_t dispSize;       // Number of items display able to show
    int8_t dispOft;         // First visible item offset on display
} Menu;

Menu *menuGet(void);

void menuSetActive(MenuIdx index);
void menuChange(int8_t diff);
bool menuIsTop(void);

MenuIdx menuGetFirstChild(void);

const char *menuGetName(MenuIdx index);
const char *menuGetValueStr(MenuIdx index);

#ifdef __cplusplus
}
#endif

#endif // MENU_H

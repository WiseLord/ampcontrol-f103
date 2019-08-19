#ifndef ACTIONS_H
#define ACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "screen.h"

#define FLAG_OFF        0
#define FLAG_ON         1

#define FLAG_EXIT       0
#define FLAG_ENTER      1
#define FLAG_SWITCH     2

#define FLAG_NEXT       -1
#define FLAG_PREV       -2

#define ACTION_HIDDEN   false
#define ACTION_VISIBLE  true

#define DIRECTION_UP    1
#define DIRECTION_DOWN  -1

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,
    ACTION_REMOTE,

    ACTION_POWERUP,
    ACTION_INIT_HW,
    ACTION_INIT_SW,
    ACTION_INIT_RTC,
    ACTION_STANDBY,

    ACTION_DISP_EXPIRED,

    ACTION_DIGIT,

    ACTION_MEDIA,

    ACTION_OPEN_MENU,
    ACTION_NAVIGATE,

    ACTION_RTC_MODE,
    ACTION_RTC_CHANGE,
    ACTION_RTC_SET_HOUR,
    ACTION_RTC_SET_MIN,
    ACTION_RTC_SET_SEC,
    ACTION_RTC_SET_DATE,
    ACTION_RTC_SET_MONTH,
    ACTION_RTC_SET_YEAR,

    ACTION_AUDIO_INPUT,
    ACTION_AUDIO_PARAM_CHANGE,

    ACTION_AUDIO_MUTE,
    ACTION_AUDIO_LOUDNESS,
    ACTION_AUDIO_SURROUND,
    ACTION_AUDIO_EFFECT3D,
    ACTION_AUDIO_BYPASS,

    ACTION_TUNER_EDIT_NAME,
    ACTION_TUNER_DEL_STATION,

    ACTION_TEXTEDIT_CHANGE,
    ACTION_TEXTEDIT_ADD_CHAR,
    ACTION_TEXTEDIT_DEL_CHAR,
    ACTION_TEXTEDIT_APPLY,
    ACTION_TEXTEDIT_CANCEL,

    ACTION_MENU_SELECT,
    ACTION_MENU_CHANGE,

    ACTION_TIMER,
    ACTION_SP_MODE,
    ACTION_SCR_DEF,

    ACTION_TYPE_END
};

typedef struct {
    ActionType type;
    bool visible;
    int16_t value;

    ScreenMode screen;
    int16_t timeout;

    ScreenMode prevScreen;
} Action;

void actionQueue(ActionType type, int16_t value);

void actionUserGet(void);
void actionHandle(bool visible);

bool actionIsDeviceActive(void);

#ifdef __cplusplus
}
#endif

#endif // ACTIONS_H

#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "screen.h"

#define FLAG_EXIT       0
#define FLAG_ENTER      1
#define FLAG_SWITCH     2

#define FLAG_NEXT       -1
#define FLAG_PREV       -2

#define DIRECTION_UP    1
#define DIRECTION_DOWN  -1

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,
    ACTION_REMOTE,

    ACTION_INIT,
    ACTION_INIT_HW,
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
    ScrMode screen;

    int16_t value;
    int16_t timeout;

    ScrMode prevScreen;
} Action;

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_INIT,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

typedef struct {
    AmpStatus status;
    uint8_t inputStatus;
} Amp;

Amp *ampGet(void);

void ampActionQueue(ActionType type, int16_t value);

void ampActionGet(void);

void ampActionHandle(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H

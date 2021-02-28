#ifndef ACTION_H
#define ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FOREACH_CMD(CMD)    \
    CMD(STBY_SWITCH)        \
    CMD(MUTE)               \
    CMD(VOL_UP)             \
    CMD(VOL_DOWN)           \
    CMD(MENU)               \
    CMD(CHAN_NEXT)          \
    CMD(CHAN_PREV)          \
    CMD(DIG_0)              \
    CMD(DIG_1)              \
    CMD(DIG_2)              \
    CMD(DIG_3)              \
    CMD(DIG_4)              \
    CMD(DIG_5)              \
    CMD(DIG_6)              \
    CMD(DIG_7)              \
    CMD(DIG_8)              \
    CMD(DIG_9)              \
    CMD(IN_NEXT)            \
    CMD(NAV_OK)             \
    CMD(NAV_BACK)           \
    CMD(NAV_RIGHT)          \
    CMD(NAV_UP)             \
    CMD(NAV_LEFT)           \
    CMD(NAV_DOWN)           \
    CMD(BASS_UP)            \
    CMD(BASS_DOWN)          \
    CMD(MIDDLE_UP)          \
    CMD(MIDDLE_DOWN)        \
    CMD(TREBLE_UP)          \
    CMD(TREBLE_DOWN)        \
    CMD(LOUDNESS)           \
    CMD(SURROUND)           \
    CMD(EFFECT_3D)          \
    CMD(TONE_BYPASS)        \
    CMD(TIME)               \
    CMD(STOP)               \
    CMD(PLAY_PAUSE)         \
    CMD(REW)                \
    CMD(FWD)                \
    CMD(TIMER)              \
    CMD(SP_MODE)            \
    CMD(SCR_DEF)            \
    CMD(BT_IN_NEXT)         \
    CMD(TUNER_MONO)         \

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,
    ACTION_REMOTE,

    ACTION_INIT_HW,
    ACTION_INIT_RTC,
    ACTION_STANDBY,

    ACTION_DISP_EXPIRED,

    ACTION_DIGIT_INPUT,
    ACTION_FINISH_DIGIT_INPUT,

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

    ACTION_AUDIO_INPUT_CHANGE,
    ACTION_AUDIO_INPUT_SET,
    ACTION_AUDIO_PARAM_CHANGE,
    ACTION_AUDIO_PARAM_SET,

    ACTION_AUDIO_MUTE,
    ACTION_AUDIO_LOUDNESS,
    ACTION_AUDIO_SURROUND,
    ACTION_AUDIO_EFFECT3D,
    ACTION_AUDIO_BYPASS,

    ACTION_RESTORE_VOLUME,

    ACTION_BT_INPUT_CHANGE,

    ACTION_TUNER_SET_FREQ,
    ACTION_TUNER_FORCE_MONO,
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

    ACTION_MPD_POWEROFF,

    ACTION_TYPE_END
};

typedef struct {
    ActionType type;
    int16_t value;
} Action;

#ifdef __cplusplus
}
#endif

#endif // ACTION_H

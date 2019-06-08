#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // CMD_H

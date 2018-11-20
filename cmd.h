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
    CMD(IN_NEXT)            \
    CMD(IN_PREV)            \
    CMD(IN_0)               \
    CMD(IN_1)               \
    CMD(IN_2)               \
    CMD(IN_3)               \
    CMD(IN_4)               \
    CMD(IN_5)               \
    CMD(IN_6)               \
    CMD(IN_7)               \
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
    CMD(STBY_ENTER)         \
    CMD(STBY_EXIT)          \

#ifdef __cplusplus
}
#endif

#endif // CMD_H

#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_CMD(CMD)    \
    CMD(STBY_SWITCH)        \
    CMD(MUTE)               \
    CMD(PARAM_UP)           \
    CMD(PARAM_DOWN)         \
    CMD(TIME)               \

#ifdef __cplusplus
}
#endif

#endif // CMD_H

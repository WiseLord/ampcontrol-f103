#ifndef ACTIONS_H
#define ACTIONS_H

#include <inttypes.h>

#define CMD_OFF         0
#define CMD_ON          1
#define CMD_SWITCH      2

#define ACTION_HIDDEN   0
#define ACTION_VISIBLE  1

typedef enum {
    ACTION_NONE,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,

    ACTION_STANDBY,

    ACTION_RTC_SHOW,
    ACTION_RTC_MODE,
    ACTION_RTC_CHANGE,
    ACTION_RTC_SET,

    ACTION_BR_WORK,
    ACTION_BR_STBY,

    ACTION_TIMER_EXPIRED,

    ACTION_TYPE_END
} ActionType;

typedef struct {
    ActionType type;
    int16_t value;
} Action;

Action actionUserGet(void);
void actionHandle(Action action, uint8_t visible);

#endif // ACTIONS_H

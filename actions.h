#ifndef ACTIONS_H
#define ACTIONS_H

#include <inttypes.h>

enum {
    ACTION_NONE,

    ACTION_BTN0,
    ACTION_BTN1,
    ACTION_BTN2,
    ACTION_BTN3,
    ACTION_BTN4,
    ACTION_BTN5,

    ACTION_STANDBY,

    ACTION_RTC,

    ACTION_DISPTIME,
};

enum {
    ACTVAL_BTN_SHORT = 0,
    ACTVAL_BTN_LONG,

    ACTVAL_STANDBY_SWITCH = 0,
    ACTVAL_STANDBY_ENTER,
    ACTVAL_STANDBY_EXIT,

    ACTVAL_RTC_SHOW = 0,
    ACTVAL_RTC_MODE,
    ACTVAL_RTC_INC,
    ACTVAL_RTC_DEC,
};

typedef struct {
    uint8_t param;
    int8_t value;
} Action;

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
    SCREEN_SPECTRUM,
} Screen;

Action actionGet();
void actionHandle(Action action);
void actionShowScreen();

#endif // ACTIONS_H

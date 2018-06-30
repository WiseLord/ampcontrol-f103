#ifndef ACTIONS_H
#define ACTIONS_H

#include <inttypes.h>

typedef enum {
    ACTION_NONE,

    ACTION_BTN0,
    ACTION_BTN1,
    ACTION_BTN2,
    ACTION_BTN3,
    ACTION_BTN4,
    ACTION_BTN5,
    ACTION_BTN0_LONG,
    ACTION_BTN1_LONG,
    ACTION_BTN2_LONG,
    ACTION_BTN3_LONG,
    ACTION_BTN4_LONG,
    ACTION_BTN5_LONG,

    ACTION_DISTIME_EXPIRED,

    ACTION_STANDBY,
    ACTION_STANDBY_ENTER,
    ACTION_STANDBY_EXIT,

    ACTION_RTC,
    ACTION_RTC_EDIT,

    ACTION_END
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

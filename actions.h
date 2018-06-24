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

    ACTION_GO_STANDBY,
    ACTION_EXIT_STANDBY,

    ACTION_NEXT_RTCMODE,

    ACTION_END
} Action;

typedef enum {
    SCREEN_STANDBY,
    SCREEN_TIME,
} Screen;

Action actionGet();
void actionHandle(Action action);
void actionShowScreen();

#endif // ACTIONS_H

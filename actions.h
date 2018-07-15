#ifndef ACTIONS_H
#define ACTIONS_H

#include <inttypes.h>

enum {
    ATYPE_NONE,

    ATYPE_BTN0,
    ATYPE_BTN1,
    ATYPE_BTN2,
    ATYPE_BTN3,
    ATYPE_BTN4,
    ATYPE_BTN5,

    ATYPE_STANDBY,

    ATYPE_RTC,

    ATYPE_DISPTIME,

    ATYPE_BRIGHTNESS
};

enum {
    AMODE_BTN_SHORT = 0,
    AMODE_BTN_LONG,

    AMODE_STANDBY_SWITCH = 0,
    AMODE_STANDBY_ENTER,
    AMODE_STANDBY_EXIT,

    AMODE_RTC_SHOW = 0,
    AMODE_RTC_MODE,
    AMODE_RTC_CHANGE,

    AMODE_BRIGHTNESS_WORK = 0,
    AMODE_BRIGNTNESS_STANDBY,
};

typedef struct {
    uint8_t type;
    uint8_t mode;
    int16_t value;
} Action;

void actionGet(void);
void actionHandle(void);
void actionShowScreen(void);

#endif // ACTIONS_H
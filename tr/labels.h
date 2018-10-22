#ifndef TR_H
#define TR_H

#include "../menu.h"

typedef enum {
    LANG_BY,
    LANG_RU,

    LANG_END
} Lang;

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_BRIGNTNESS,

    LABEL_VOLUME,
    LABEL_BASS,
    LABEL_MIDDLE,
    LABEL_TREBLE,
    LABEL_FRONTREAR,
    LABEL_BALANCE,
    LABEL_CENTER,
    LABEL_SUBWOOFER,
    LABEL_PREAMP,

    LABEL_GAIN0,
    LABEL_GAIN1,
    LABEL_GAIN2,
    LABEL_GAIN3,
    LABEL_GAIN4,
    LABEL_GAIN5,
    LABEL_GAIN6,
    LABEL_GAIN7,

    // Menu
    LABEL_MENU,
    LABEL_MENU_END = LABEL_MENU + (MENU_END - MENU_NULL - 1),

    LABEL_END
} Label;

void labelsSetLang(Lang value);
const char **labelsGet(void);

void labelsInit(void);

#endif // TR_H

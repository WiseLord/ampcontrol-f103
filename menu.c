#include "menu.h"
#include "tr/labels.h"

static Menu menu;

static const MenuItem menuItems[MENU_END] = {
    //                          parent
    [MENU_SETUP]            = { MENU_NULL,          },

    [MENU_SETUP_AUDIO]      = { MENU_SETUP,         },
    [MENU_SETUP_TUNER]      = { MENU_SETUP,         },
    [MENU_SETUP_SPECTRUM]   = { MENU_SETUP,         },
    [MENU_SETUP_DISPLAY]    = { MENU_SETUP,         },
    [MENU_SETUP_INPUT]      = { MENU_SETUP,         },

    [MENU_TUNER_IC]         = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_FREQ_MIN]   = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_FREQ_MAX]   = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_STEP]       = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_MONO]       = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_RDS]        = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_BASS]       = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_BAND]       = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_DE]         = { MENU_SETUP_TUNER,   },
    [MENU_TUNER_SOFTMUTE]   = { MENU_SETUP_TUNER,   },
};

void menuInit()
{
    menuSetActive(MENU_TUNER_FREQ_MAX);
}

char *menuGetName(MenuIdx index)
{
    const char **txtLabels = labelsGet();

    return (char *)txtLabels[LABEL_MENU + (index - MENU_NULL)];
}

void menuSetActive(MenuIdx index)
{
    menu.active = index;
    menu.parent = menuItems[index].parent;

    uint8_t idx;

    for (idx = 0; idx < MENU_MAX_LEN; idx++) {
        menu.idx[idx] = 0;
    }

    idx = 0;
    for (MenuIdx item = 0; item < MENU_END; item++) {
        if (menuItems[item].parent == menu.parent) {
            menu.idx[idx++] = item;
            if (idx >= MENU_MAX_LEN)
                break;
        }
    }
    menu.size = idx;
}

Menu *menuGet(void)
{
    return &menu;
}

MenuIdx menuGetParent(MenuIdx index)
{
    return menuItems[index].parent;
}

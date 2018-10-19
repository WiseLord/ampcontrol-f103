#include "labels.h"

static Lang lang = LANG_END;

extern char *const labels_by[LABEL_END];
extern char *const labels_ru[LABEL_END];

const char *labels_default[LABEL_END] = {
    [LABEL_SUNDAY]          = "SUNDAY",
    [LABEL_MONDAY]          = "MONDAY",
    [LABEL_TUESDAY]         = "TUESDAY",
    [LABEL_WEDNESDAY]       = "WEDNESDAY",
    [LABEL_THURSDAY]        = "THURSDAY",
    [LABEL_FRIDAY]          = "FRIDAY",
    [LABEL_SATURDAY]        = "SATURDAY",

    [LABEL_BRIGNTNESS]      = "Brightness",

    [LABEL_VOLUME]          = "Volume",
    [LABEL_BASS]            = "Bass",
    [LABEL_MIDDLE]          = "Middle",
    [LABEL_TREBLE]          = "Treble",
    [LABEL_FRONTREAR]       = "Front/rear",
    [LABEL_BALANCE]         = "Balance",
    [LABEL_CENTER]          = "Center",
    [LABEL_SUBWOOFER]       = "Subwoofer",
    [LABEL_PREAMP]          = "Preamp",

    [LABEL_GAIN0]           = "FM tuner",
    [LABEL_GAIN1]           = "Computer",
    [LABEL_GAIN2]           = "TV",
    [LABEL_GAIN3]           = "Bluetooth",
    [LABEL_GAIN4]           = "DVD-player",
    [LABEL_GAIN5]           = "Input 5",
    [LABEL_GAIN6]           = "Input 6",
    [LABEL_GAIN7]           = "Input 7",
};

const char *labels[LABEL_END];

static void labelsFill(char *const *src)
{
    for (Label l = 0; l < LABEL_END; l++) {
        labels[l] = (src[l] ? src[l] : labels_default[l]);
    }
}

void labelsSetLang(Lang value)
{
    lang = value;
    // TODO: Save in settings
}

const char **labelsGet()
{
    switch (lang) {
    case LANG_BY:
        labelsFill(labels_by);
        break;
    case LANG_RU:
        labelsFill(labels_ru);
        break;
    default:
        break;
    }

    return labels;
}

void labelsInit(void)
{
    // TODO: Read from settings
    lang = LANG_BY;
}

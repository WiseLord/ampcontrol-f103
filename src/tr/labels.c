#include "labels.h"

#include "rc.h"
#include "settings.h"

static Lang lang = LANG_END;

typedef struct {
    const char *const *labels;
    const char *lang;
} LangMap;

static const LangMap langMap[LANG_END] = {
    [LANG_EN] = { labels_en,  "English", },
    [LANG_BY] = { labels_by,  "Беларуская"},
    [LANG_RU] = { labels_ru,  "Русский"},
    [LANG_UA] = { labels_ua,  "Українська"},
    [LANG_TR] = { labels_tr,  "Türkçe"},
//    [LANG_FR] = { labels_fr,  "Français" },
//    [LANG_RO] = { labels_ro,  "Romaneste"},
};

void labelsSetLang(Lang value)
{
    lang = value;
}

Lang labelsGetLang(void)
{
    return  lang;
}

const char *labelsGetLangName(Lang value)
{
    return langMap[value].lang;
}

const char *labelsGetDefault(Label value)
{
    if (langMap[LANG_DEFAULT].labels[value]) {
        return langMap[LANG_DEFAULT].labels[value];
    } else {
        return "???";
    }
}

const char *labelsGet(Label value)
{
    if (langMap[lang].labels[value]) {
        return langMap[lang].labels[value];
    } else {
        return labelsGetDefault(value);
    }
}

void labelsInit(void)
{
    lang = (Lang)(settingsRead(PARAM_SYSTEM_LANG, LANG_DEFAULT));
}

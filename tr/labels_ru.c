#include "labels.h"

const char *const labels_ru[LABEL_END] = {
    [LABEL_SUNDAY]          = "ВОСКРЕСЕНЬЕ",
    [LABEL_MONDAY]          = "ПОНЕДЕЛЬНИК",
    [LABEL_TUESDAY]         = "ВТОРНИК",
    [LABEL_WEDNESDAY]       = "СРЕДА",
    [LABEL_THURSDAY]        = "ЧЕТВЕРГ",
    [LABEL_FRIDAY]          = "ПЯТНИЦА",
    [LABEL_SATURDAY]        = "СУББОТА",

    [LABEL_BRIGNTNESS]      = "Яркость",

    [LABEL_VOLUME]          = "Громкость",
    [LABEL_BASS]            = "Тембр НЧ",
    [LABEL_MIDDLE]          = "Тембр СЧ",
    [LABEL_TREBLE]          = "Тембр ВЧ",
    [LABEL_FRONTREAR]       = "Фронт/тыл",
    [LABEL_BALANCE]         = "Баланс",
    [LABEL_CENTER]          = "Центр",
    [LABEL_SUBWOOFER]       = "Сабвуфер",
    [LABEL_PREAMP]          = "Усиление",

    [LABEL_BOOL_OFF]        = "ВЫКЛ",
    [LABEL_BOOL_ON]         = "ВКЛ",

    [LABEL_MENU + MENU_NULL]            = "Назад",

    [LABEL_MENU + MENU_AUDIO_IN_0]      = "Вход 0",
    [LABEL_MENU + MENU_AUDIO_IN_1]      = "Вход 1",
    [LABEL_MENU + MENU_AUDIO_IN_2]      = "Вход 2",
    [LABEL_MENU + MENU_AUDIO_IN_3]      = "Вход 3",
    [LABEL_MENU + MENU_AUDIO_IN_4]      = "Вход 4",
    [LABEL_MENU + MENU_AUDIO_IN_5]      = "Вход 5",
    [LABEL_MENU + MENU_AUDIO_IN_6]      = "Вход 6",
    [LABEL_MENU + MENU_AUDIO_IN_7]      = "Вход 7",

    [LABEL_MENU + MENU_SETUP]           = "Настройки",
    [LABEL_MENU + MENU_SETUP_LANG]      = "Язык",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Аудио",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Тюнер",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Спектр",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Дисплей",
    [LABEL_MENU + MENU_SETUP_INPUT]     = "Ввод",
    [LABEL_MENU + MENU_SETUP_RC]        = "Пульт",
};

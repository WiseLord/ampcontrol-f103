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

    [LABEL_GAIN0]           = "FM тюнер",
    [LABEL_GAIN1]           = "Компьютер",
    [LABEL_GAIN2]           = "Телевизор",
    [LABEL_GAIN3]           = "Bluetooth",
    [LABEL_GAIN4]           = "DVD-плеер",
    [LABEL_GAIN5]           = "Вход 5",
    [LABEL_GAIN6]           = "Вход 6",
    [LABEL_GAIN7]           = "Вход 7",


    [LABEL_BOOL_OFF]        = "ВКЛ",
    [LABEL_BOOL_ON]         = "ВЫКЛ",

    [LABEL_MENU + MENU_NULL]            = "Назад",

    [LABEL_MENU + MENU_SETUP]           = "Настройки",
    [LABEL_MENU + MENU_SETUP_LANG]      = "Язык",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Аудио",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Тюнер",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Спектр",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Дисплей",
    [LABEL_MENU + MENU_SETUP_INPUT]     = "Ввод",
    [LABEL_MENU + MENU_SETUP_RC]        = "Пульт",
};

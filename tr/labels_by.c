#include "labels.h"

const char * const labels_by[LABEL_END] = {
    [LABEL_SUNDAY]          = "НЯДЗЕЛЯ",
    [LABEL_MONDAY]          = "ПАНЯДЗЕЛАК",
    [LABEL_TUESDAY]         = "АЎТОРАК",
    [LABEL_WEDNESDAY]       = "СЕРАДА",
    [LABEL_THURSDAY]        = "ЧАЦВЕР",
    [LABEL_FRIDAY]          = "ПЯТНІЦА",
    [LABEL_SATURDAY]        = "СУБОТА",

    [LABEL_BRIGNTNESS]      = "Яркасць",

    [LABEL_VOLUME]          = "Гучнасць",
    [LABEL_BASS]            = "Тэмбр НЧ",
    [LABEL_MIDDLE]          = "Тэмбр СЧ",
    [LABEL_TREBLE]          = "Тэмбр ВЧ",
    [LABEL_FRONTREAR]       = "Фронт/тыл",
    [LABEL_BALANCE]         = "Баланс",
    [LABEL_CENTER]          = "Цэнтр",
    [LABEL_SUBWOOFER]       = "Сабвуфер",
    [LABEL_PREAMP]          = "Пасіленне",

    [LABEL_GAIN0]           = "FM цюнэр",
    [LABEL_GAIN1]           = "Камп'ютар",
    [LABEL_GAIN2]           = "Тэлевізар",
    [LABEL_GAIN3]           = "Bluetooth",
    [LABEL_GAIN4]           = "DVD-плэер",
    [LABEL_GAIN5]           = "Уваход 5",
    [LABEL_GAIN6]           = "Уваход 6",
    [LABEL_GAIN7]           = "Уваход 7",

    [LABEL_BOOL_OFF]        = "ВЫКЛ",
    [LABEL_BOOL_ON]         = "УКЛ",

    [LABEL_MENU + MENU_NULL]            = "Назад",

    [LABEL_MENU + MENU_SETUP]           = "Налады",
    [LABEL_MENU + MENU_SETUP_LANG]      = "Мова",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Аудыё",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Цюнер",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Спектр",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Дысплэй",
    [LABEL_MENU + MENU_SETUP_INPUT]     = "Увод",
    [LABEL_MENU + MENU_SETUP_RC]        = "Пульт",
};

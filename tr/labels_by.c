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

    [LABEL_IN_TUNER]        = "Цюнер",
    [LABEL_IN_PC]           = "Кампутар",
    [LABEL_IN_TV]           = "Тэлевізар",
    [LABEL_IN_BLUETOOTH]    = "Bluetooth",
//    [LABEL_IN_DVD]          = "DVD",
//    [LABEL_IN_USB]          = "USB",
    [LABEL_IN_MICROPHONE]   = "Мікрафон",
    [LABEL_IN_GUITAR]       = "Гітара",
    [LABEL_IN_TURNTABLES]   = "Круцёлкі",
    [LABEL_IN_CASSETTE]     = "Касета",
    [LABEL_IN_PROJECTOR]    = "Праектар",
    [LABEL_IN_SATELLITE]    = "Спадарожнік",
    [LABEL_IN_MIXER]        = "Міксер",

    [LABEL_BOOL_OFF]        = "ВЫКЛ",
    [LABEL_BOOL_ON]         = "УКЛ",

    [LABEL_TUNER_IC + TUNER_IC_NO]      = "Няма",
//    [LABEL_TUNER_IC + TUNER_IC_RDA5807] = "RDA5807",
//    [LABEL_TUNER_IC + TUNER_IC_SI4703]  = "Si4703",
//    [LABEL_TUNER_IC + TUNER_IC_TEA5767] = "TEA5767",

//    [LABEL_TUNER_BAND + TUNER_BAND_FM_US_EUROPE]    = "US/Europe FM",
//    [LABEL_TUNER_BAND + TUNER_BAND_FM_JAPAN]        = "Japan FM",
//    [LABEL_TUNER_BAND + TUNER_BAND_FM_WORLDWIDE]    = "Worldwide FM",
//    [LABEL_TUNER_BAND + TUNER_BAND_FM_EASTEUROPE]   = "East Europe FM",

    [LABEL_TUNER_STEP + TUNER_STEP_50K]     = "50кГц",
    [LABEL_TUNER_STEP + TUNER_STEP_100K]    = "100кГц",
    [LABEL_TUNER_STEP + TUNER_STEP_200K]    = "200кГц",

    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_50u] = "50мкс",
    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_75u] = "75мкс",

    [LABEL_AUDIO_IC + AUDIO_IC_NO]      = "Няма",
//    [LABEL_AUDIO_IC + AUDIO_IC_TDA7439] = "TDA7439",
//    [LABEL_AUDIO_IC + AUDIO_IC_TDA7313] = "TDA7313",
//    [LABEL_AUDIO_IC + AUDIO_IC_PT232X]  = "PT232x",

//    [LABEL_LANG + LANG_DEFAULT]         = "English",
//    [LABEL_LANG + LANG_BY]              = "Беларуская",
//    [LABEL_LANG + LANG_RU]              = "Русский",

    [LABEL_MENU + MENU_NULL]            = "Назад",

    [LABEL_MENU + MENU_AUDIO_IC]        = "Аўдыёпрацэсар",

    [LABEL_MENU + MENU_SETUP]           = "Налады",
    [LABEL_MENU + MENU_SETUP_LANG]      = "Мова",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Аудыё",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Цюнер",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Спектр",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Дысплэй",
    [LABEL_MENU + MENU_SETUP_INPUT]     = "Увод",
    [LABEL_MENU + MENU_SETUP_RC]        = "Пульт",

    [LABEL_MENU + MENU_AUDIO_IN_0]      = "Уваход 0",
    [LABEL_MENU + MENU_AUDIO_IN_1]      = "Уваход 1",
    [LABEL_MENU + MENU_AUDIO_IN_2]      = "Уваход 2",
    [LABEL_MENU + MENU_AUDIO_IN_3]      = "Уваход 3",
    [LABEL_MENU + MENU_AUDIO_IN_4]      = "Уваход 4",
    [LABEL_MENU + MENU_AUDIO_IN_5]      = "Уваход 5",
    [LABEL_MENU + MENU_AUDIO_IN_6]      = "Уваход 6",
    [LABEL_MENU + MENU_AUDIO_IN_7]      = "Уваход 7",

    [LABEL_MENU + MENU_TUNER_IC]        = "Чып цюнера",
    [LABEL_MENU + MENU_TUNER_BAND]      = "Дыяпазон",
    [LABEL_MENU + MENU_TUNER_STEP]      = "Крок",
    [LABEL_MENU + MENU_TUNER_DEEMPH]    = "Deemphasys",
    [LABEL_MENU + MENU_TUNER_FMONO]     = "Мона прымусова",
    [LABEL_MENU + MENU_TUNER_RDS]       = "Уключыць RDS",
    [LABEL_MENU + MENU_TUNER_BASS]      = "Узмацніць НЧ",
    [LABEL_MENU + MENU_TUNER_VOLUME]    = "Гучнасць",

    [LABEL_MENU + MENU_SPECTURM_MODE]   = "Рэжым экрана",
//    [LABEL_MENU + MENU_SPECTRUM_SPEED]  = "Хуткасць",

    [LABEL_MENU + MENU_DISPLAY_BR_STBY] = "Яркасць STBY",
    [LABEL_MENU + MENU_DISPLAY_ROTATE]  = "Круціць",

    [LABEL_MENU + MENU_INPUT_ENC_RES]   = "Вырашэнне энкодера",

};

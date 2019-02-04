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
    [LABEL_MENU + MENU_SETUP_MUTESTBY]  = "Выводы Mute и Stby",

    [LABEL_VOLUME]          = "Громкость",
    [LABEL_BASS]            = "Тембр НЧ",
    [LABEL_MIDDLE]          = "Тембр СЧ",
    [LABEL_TREBLE]          = "Тембр ВЧ",
    [LABEL_FRONTREAR]       = "Фронт/тыл",
    [LABEL_BALANCE]         = "Баланс",
    [LABEL_CENTER]          = "Центр",
    [LABEL_SUBWOOFER]       = "Сабвуфер",
    [LABEL_PREAMP]          = "Усиление",

    [LABEL_IN_TUNER]        = "Тюнер",
    [LABEL_IN_PC]           = "Компьютер",
    [LABEL_IN_TV]           = "Телевизор",
//    [LABEL_IN_BLUETOOTH]    = "Bluetooth",
//    [LABEL_IN_DVD]          = "DVD",
//    [LABEL_IN_USB]          = "USB",
    [LABEL_IN_MICROPHONE]   = "Микрофон",
    [LABEL_IN_GUITAR]       = "Гитара",
    [LABEL_IN_TURNTABLES]   = "Вертушки",
    [LABEL_IN_CASSETTE]     = "Кассета",
    [LABEL_IN_PROJECTOR]    = "Проектор",
    [LABEL_IN_SATELLITE]    = "Спутник",
    [LABEL_IN_MIXER]        = "Микшер",

    [LABEL_BOOL_OFF]        = "ВЫКЛ",
    [LABEL_BOOL_ON]         = "ВКЛ",

    [LABEL_TUNER_FM_STATION_NAME]   = "Имя FM станции",

    [LABEL_TUNER_IC + TUNER_IC_NO]        = "Нет",

//    [LABEL_TUNER_BAND + TUNER_BAND_FM_US_EUROPE]    = "US/Europe FM",
//    [LABEL_TUNER_BAND + TUNER_BAND_FM_JAPAN]        = "Japan FM",
//    [LABEL_TUNER_BAND + TUNER_BAND_FM_WORLDWIDE]    = "Worldwide FM",
//    [LABEL_TUNER_BAND + TUNER_BAND_FM_EASTEUROPE]   = "East Europe FM",

    [LABEL_TUNER_STEP + TUNER_STEP_50K]     = "50кГц",
    [LABEL_TUNER_STEP + TUNER_STEP_100K]    = "100кГц",
    [LABEL_TUNER_STEP + TUNER_STEP_200K]    = "200кГц",

    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_50u] = "50мкс",
    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_75u] = "75мкс",

    [LABEL_TUNER_MODE + TUNER_MODE_GRID]        = "Сетка",
    [LABEL_TUNER_MODE + TUNER_MODE_STATIONS]    = "Станции",
    [LABEL_TUNER_MODE + TUNER_MODE_SCAN]        = "Поиск",

    [LABEL_SPECTRUM_MODE + SP_MODE_STEREO]      = "Стерео",
    [LABEL_SPECTRUM_MODE + SP_MODE_MIXED]       = "Смешанно",
    [LABEL_SPECTRUM_MODE + SP_MODE_WATERFALL]   = "Водопад",

    [LABEL_AUDIO_IC + AUDIO_IC_NO]      = "Нет",

    [LABEL_MENU + MENU_NULL]            = "Назад",

    [LABEL_MENU + MENU_SETUP]           = "Настройки",
    [LABEL_MENU + MENU_SETUP_LANG]      = "Язык",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Аудио",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Тюнер",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Спектр",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Дисплей",
    [LABEL_MENU + MENU_SETUP_INPUT]     = "Ввод",
    [LABEL_MENU + MENU_SETUP_RC]        = "Пульт",

    [LABEL_MENU + MENU_AUDIO_IC]        = "Аудиопроцессор",

    [LABEL_MENU + MENU_AUDIO_IN_0]      = "Вход 0",
    [LABEL_MENU + MENU_AUDIO_IN_1]      = "Вход 1",
    [LABEL_MENU + MENU_AUDIO_IN_2]      = "Вход 2",
    [LABEL_MENU + MENU_AUDIO_IN_3]      = "Вход 3",
    [LABEL_MENU + MENU_AUDIO_IN_4]      = "Вход 4",
    [LABEL_MENU + MENU_AUDIO_IN_5]      = "Вход 5",
    [LABEL_MENU + MENU_AUDIO_IN_6]      = "Вход 6",
    [LABEL_MENU + MENU_AUDIO_IN_7]      = "Вход 7",

    [LABEL_MENU + MENU_TUNER_IC]        = "Чип тюнера",
    [LABEL_MENU + MENU_TUNER_BAND]      = "Диапазон",
    [LABEL_MENU + MENU_TUNER_STEP]      = "Шаг",
    [LABEL_MENU + MENU_TUNER_DEEMPH]    = "Предыскажение",
    [LABEL_MENU + MENU_TUNER_MODE]      = "Режим настройки",
    [LABEL_MENU + MENU_TUNER_FMONO]     = "Форсировать моно",
    [LABEL_MENU + MENU_TUNER_RDS]       = "Включить RDS",
    [LABEL_MENU + MENU_TUNER_BASS]      = "Усилить НЧ",
    [LABEL_MENU + MENU_TUNER_VOLUME]    = "Громкость",

    [LABEL_MENU + MENU_SPECTURM_MODE]   = "Режим экрана",

    [LABEL_MENU + MENU_DISPLAY_BR_STBY] = "Яркость STBY",
    [LABEL_MENU + MENU_DISPLAY_ROTATE]  = "Развернуть",

    [LABEL_MENU + MENU_INPUT_ENC_RES]   = "Разрешение энкодера",
};

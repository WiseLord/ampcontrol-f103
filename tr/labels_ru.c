#include "labels.h"

const char *const labels_ru[LABEL_END] = {
    [LABEL_SUNDAY]          = "ВОСКРЕСЕНЬЕ",
    [LABEL_MONDAY]          = "ПОНЕДЕЛЬНИК",
    [LABEL_TUESDAY]         = "ВТОРНИК",
    [LABEL_WEDNESDAY]       = "СРЕДА",
    [LABEL_THURSDAY]        = "ЧЕТВЕРГ",
    [LABEL_FRIDAY]          = "ПЯТНИЦА",
    [LABEL_SATURDAY]        = "СУББОТА",

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

    [LABEL_TUNER_BAND + TUNER_BAND_FM_US_EUROPE]    = "США/Европа FM",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_JAPAN]        = "Япония FM",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_WORLDWIDE]    = "Мировой FM",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_EASTEUROPE]   = "Вост. Европа FM",

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

    [LABEL_ALARM_DAY + ALARM_DAY_OFF]       = "Выкл",
    [LABEL_ALARM_DAY + ALARM_DAY_WEEKDAYS]  = "Будни",
    [LABEL_ALARM_DAY + ALARM_DAY_ALL_DAYS]  = "Все дни",

//    [LABEL_MUTESTBY + MUTESTBY_SWD] = "SWD",
    [LABEL_MUTESTBY + MUTESTBY_POS] = "Актив. 1",
    [LABEL_MUTESTBY + MUTESTBY_NEG] = "Актив. 0",

    // NOTE: Keep in sync with MenuIdx in menu.h
    [LABEL_MENU + MENU_NULL]            = "Назад",

    [LABEL_MENU + MENU_SETUP]           = "Настройки",

    [LABEL_MENU + MENU_SETUP_SYSTEM]    = "Система",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Аудио",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Тюнер",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Спектр",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Дисплей",
    [LABEL_MENU + MENU_SETUP_ALARM]     = "Будильник",
    [LABEL_MENU + MENU_SETUP_RC]        = "Пульт",

    [LABEL_MENU + MENU_SYSTEM_LANG]     = "Язык",
    [LABEL_MENU + MENU_SYSTEM_MUTESTBY] = "Mute и Stby",
    [LABEL_MENU + MENU_SYSTEM_ENC_RES]  = "Разрешение энкодера",
    [LABEL_MENU + MENU_SYSTEM_SIL_TIM]  = "Таймер тишины",
    [LABEL_MENU + MENU_SYSTEM_RTC_CORR] = "Коррекция времени",

    [LABEL_MENU + MENU_AUDIO_IC]        = "Аудиопроцессор",
    [LABEL_MENU + MENU_AUDIO_IN]        = "Вход",

    [LABEL_MENU + MENU_TUNER_IC]        = "Чип тюнера",
    [LABEL_MENU + MENU_TUNER_BAND]      = "Диапазон",
    [LABEL_MENU + MENU_TUNER_STEP]      = "Шаг",
    [LABEL_MENU + MENU_TUNER_DEEMPH]    = "Предыскажение",
    [LABEL_MENU + MENU_TUNER_MODE]      = "Режим настройки",
    [LABEL_MENU + MENU_TUNER_FMONO]     = "Форсировать моно",
    [LABEL_MENU + MENU_TUNER_RDS]       = "Включить RDS",
    [LABEL_MENU + MENU_TUNER_BASS]      = "Усилить НЧ",
    [LABEL_MENU + MENU_TUNER_VOLUME]    = "Громкость",

    [LABEL_MENU + MENU_ALARM_HOUR]      = "Час",
    [LABEL_MENU + MENU_ALARM_MINUTE]    = "Минута",
    [LABEL_MENU + MENU_ALARM_DAYS]      = "Дни",

    [LABEL_MENU + MENU_SPECTURM_MODE]   = "Режим экрана",
    [LABEL_MENU + MENU_SPECTRUM_PEAKS]  = "Пиковый уровень",

    [LABEL_MENU + MENU_DISPLAY_BR_STBY] = "Яркость STBY",
    [LABEL_MENU + MENU_DISPLAY_BR_WORK] = "Яркость",
    [LABEL_MENU + MENU_DISPLAY_ROTATE]  = "Развернуть",

    // NOTE: Keep in sync with cmd.h define list
    [LABEL_MENU + MENU_RC_STBY_SWITCH]  = "Режим ожидания",
    [LABEL_MENU + MENU_RC_MUTE]         = "Приглушение",
    [LABEL_MENU + MENU_RC_VOL_UP]       = "Громкость +",
    [LABEL_MENU + MENU_RC_VOL_DOWN]     = "Громкость -",
    [LABEL_MENU + MENU_RC_MENU]         = "Меню",
    [LABEL_MENU + MENU_RC_CHAN_NEXT]    = "След. канал",
    [LABEL_MENU + MENU_RC_CHAN_PREV]    = "Пред. канал",
    [LABEL_MENU + MENU_RC_DIG]          = "Цифра",
    [LABEL_MENU + MENU_RC_IN_NEXT]      = "След. вход",
    [LABEL_MENU + MENU_RC_NAV_OK]       = "OK",
    [LABEL_MENU + MENU_RC_NAV_BACK]     = "Назад",
    [LABEL_MENU + MENU_RC_NAV_RIGHT]    = "Вправо",
    [LABEL_MENU + MENU_RC_NAV_UP]       = "Вверх",
    [LABEL_MENU + MENU_RC_NAV_LEFT]     = "Влево",
    [LABEL_MENU + MENU_RC_NAV_DOWN]     = "Вниз",
    [LABEL_MENU + MENU_RC_BASS_UP]      = "НЧ +",
    [LABEL_MENU + MENU_RC_BASS_DOWN]    = "НЧ -",
    [LABEL_MENU + MENU_RC_MIDDLE_UP]    = "СЧ +",
    [LABEL_MENU + MENU_RC_MIDDLE_DOWN]  = "СЧ -",
    [LABEL_MENU + MENU_RC_TREBLE_UP]    = "ВЧ +",
    [LABEL_MENU + MENU_RC_TREBLE_DOWN]  = "ВЧ -",
    [LABEL_MENU + MENU_RC_LOUDNESS]     = "Тонкоррекция",
    [LABEL_MENU + MENU_RC_SURROUND]     = "Звук вокруг",
    [LABEL_MENU + MENU_RC_EFFECT_3D]    = "3D эффект",
    [LABEL_MENU + MENU_RC_TONE_BYPASS]  = "Отключить тон",
    [LABEL_MENU + MENU_RC_TIME]         = "Час",
    [LABEL_MENU + MENU_RC_STOP]         = "Стоп",
    [LABEL_MENU + MENU_RC_PLAY]         = "Играть",
    [LABEL_MENU + MENU_RC_PAUSE]        = "Пауза",
    [LABEL_MENU + MENU_RC_REW]          = "Шаг назад",
    [LABEL_MENU + MENU_RC_FWD]          = "Шаг вперёд",
    [LABEL_MENU + MENU_RC_TIMER]        = "Таймер",
};

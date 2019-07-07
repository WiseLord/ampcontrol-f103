#include "labels.h"

const char *const labels_fr[LABEL_END] = {
    [LABEL_SUNDAY]          = "DIMANCHE",
    [LABEL_MONDAY]          = "LUNDI",
    [LABEL_TUESDAY]         = "MARDI",
    [LABEL_WEDNESDAY]       = "MERCREDI",
    [LABEL_THURSDAY]        = "JEUDI",
    [LABEL_FRIDAY]          = "VENDREDI",
    [LABEL_SATURDAY]        = "SAMEDI",

//    [LABEL_VOLUME]          = "Volume",
    [LABEL_BASS]            = "Basses",
    [LABEL_MIDDLE]          = "Médiums",
    [LABEL_TREBLE]          = "Aigus",
    [LABEL_FRONTREAR]       = "Avant/arrière",
//    [LABEL_BALANCE]         = "Balance",
    [LABEL_CENTER]          = "Centre",
    [LABEL_SUBWOOFER]       = "Caisson de basses",
    [LABEL_PREAMP]          = "Préamp",

    [LABEL_IN_TUNER]        = "Radio",
    [LABEL_IN_PC]           = "Ordinateur",
    [LABEL_IN_TV]           = "Téléviseur",
    [LABEL_IN_BLUETOOTH]    = "Bluetooth",
//    [LABEL_IN_DVD]          = "DVD",
//    [LABEL_IN_USB]          = "USB",
//    [LABEL_IN_MICROPHONE]   = "Microphone",
    [LABEL_IN_GUITAR]       = "Guitare",
    [LABEL_IN_TURNTABLES]   = "Platine disque",
    [LABEL_IN_CASSETTE]     = "Cassette",
    [LABEL_IN_PROJECTOR]    = "Projecteur",
    [LABEL_IN_SATELLITE]    = "Satellite",
    [LABEL_IN_MIXER]        = "Mélangeur",
//    [LABEL_IN_KARADIO]      = "KaRadio",

//    [LABEL_BOOL_OFF]        = "OFF",
//    [LABEL_BOOL_ON]         = "ON",

    [LABEL_TUNER_FM_STATION_NAME]   = "Nom de la station",

    [LABEL_TUNER_IC + TUNER_IC_NO]      = "Aucun",

    [LABEL_TUNER_BAND + TUNER_BAND_FM_US_EUROPE]    = "FM US/Europe",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_JAPAN]        = "FM Japonnaise",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_WORLDWIDE]    = "FM mondiale",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_EASTEUROPE]   = "FM Europe de l'est",

//    [LABEL_TUNER_STEP + TUNER_STEP_50K]     = "50kHz",
//    [LABEL_TUNER_STEP + TUNER_STEP_100K]    = "100kHz",
//    [LABEL_TUNER_STEP + TUNER_STEP_200K]    = "200kHz",

//    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_50u] = "50us",
//    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_75u] = "75us",

    [LABEL_TUNER_MODE + TUNER_MODE_GRID]        = "Grille",
    [LABEL_TUNER_MODE + TUNER_MODE_STATIONS]    = "Stations",
    [LABEL_TUNER_MODE + TUNER_MODE_SCAN]        = "Recherche",

    [LABEL_SPECTRUM_MODE + SP_MODE_STEREO]      = "Stéréo",
    [LABEL_SPECTRUM_MODE + SP_MODE_MIXED]       = "Mélangé",
    [LABEL_SPECTRUM_MODE + SP_MODE_WATERFALL]   = "Cascade",

    [LABEL_AUDIO_IC + AUDIO_IC_NO]      = "Aucun",

    [LABEL_ALARM_DAY + ALARM_DAY_OFF]       = "Off",
    [LABEL_ALARM_DAY + ALARM_DAY_WEEKDAYS]  = "Jours de semaine",
    [LABEL_ALARM_DAY + ALARM_DAY_ALL_DAYS]  = "Tous les jours",

    [LABEL_MUTESTBY + MUTESTBY_SWD] = "SWD",
    [LABEL_MUTESTBY + MUTESTBY_POS] = "Activation 1",
    [LABEL_MUTESTBY + MUTESTBY_NEG] = "Activation 0",

    // NOTE: Keep in sync with MenuIdx in menu.h
    [LABEL_MENU + MENU_NULL]            = "Retour",

    [LABEL_MENU + MENU_SETUP]           = "Paramètres",

    [LABEL_MENU + MENU_SETUP_SYSTEM]    = "Système",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Audio",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Radio",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Spectre",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Ecran",
    [LABEL_MENU + MENU_SETUP_ALARM]     = "Alarme",
    [LABEL_MENU + MENU_SETUP_RC]        = "Télécommande",

    [LABEL_MENU + MENU_SYSTEM_LANG]     = "Language",
    [LABEL_MENU + MENU_SYSTEM_MUTESTBY] = "Silence & pause",
    [LABEL_MENU + MENU_SYSTEM_ENC_RES]  = "Résolution encodeur",
    [LABEL_MENU + MENU_SYSTEM_SIL_TIM]  = "Durée du silence",
    [LABEL_MENU + MENU_SYSTEM_RTC_CORR] = "Correction du temps",

    [LABEL_MENU + MENU_AUDIO_IC]        = "Processeur audio",
    [LABEL_MENU + MENU_AUDIO_IN]        = "Entrée",

    [LABEL_MENU + MENU_TUNER_IC]        = "Récepteur IC",
    [LABEL_MENU + MENU_TUNER_BAND]      = "Bande",
    [LABEL_MENU + MENU_TUNER_STEP]      = "Saut du scan",
    [LABEL_MENU + MENU_TUNER_DEEMPH]    = "De-emphasis",
    [LABEL_MENU + MENU_TUNER_MODE]      = "Mode de contrôle",
    [LABEL_MENU + MENU_TUNER_FMONO]     = "Forcer mode mono",
    [LABEL_MENU + MENU_TUNER_RDS]       = "Activation RDS",
    [LABEL_MENU + MENU_TUNER_BASS]      = "Réhausse des basses",
    [LABEL_MENU + MENU_TUNER_VOLUME]    = "Volume",

    [LABEL_MENU + MENU_ALARM_HOUR]      = "Heures",
    [LABEL_MENU + MENU_ALARM_MINUTE]    = "Minutes",
    [LABEL_MENU + MENU_ALARM_DAYS]      = "Jours",

    [LABEL_MENU + MENU_SPECTURM_MODE]   = "Mode écran",
//    [LABEL_MENU + MENU_SPECTURM_PEAKS]  = "Peak level",

    [LABEL_MENU + MENU_DISPLAY_BR_STBY] = "Eclairage STBY",
    [LABEL_MENU + MENU_DISPLAY_BR_WORK] = "Eclairage",
    [LABEL_MENU + MENU_DISPLAY_ROTATE]  = "Rotation",

    // NOTE: Keep in sync with cmd.h define list
    [LABEL_MENU + MENU_RC_STBY_SWITCH]  = "Interrupteur standby",
    [LABEL_MENU + MENU_RC_MUTE]         = "Silence",
    [LABEL_MENU + MENU_RC_VOL_UP]       = "Montée du Volume",
    [LABEL_MENU + MENU_RC_VOL_DOWN]     = "Descente du volume",
    [LABEL_MENU + MENU_RC_MENU]         = "Menu",
    [LABEL_MENU + MENU_RC_CHAN_NEXT]    = "Canal suivant",
    [LABEL_MENU + MENU_RC_CHAN_PREV]    = "Canal précédent",
    [LABEL_MENU + MENU_RC_DIG]          = "Chiffre",
    [LABEL_MENU + MENU_RC_IN_NEXT]      = "Entrée suivante",
    [LABEL_MENU + MENU_RC_NAV_OK]       = "OK",
    [LABEL_MENU + MENU_RC_NAV_BACK]     = "Retour",
    [LABEL_MENU + MENU_RC_NAV_RIGHT]    = "Droite",
    [LABEL_MENU + MENU_RC_NAV_UP]       = "Montée",
    [LABEL_MENU + MENU_RC_NAV_LEFT]     = "Gauche",
    [LABEL_MENU + MENU_RC_NAV_DOWN]     = "Descente",
    [LABEL_MENU + MENU_RC_BASS_UP]      = "Montée basses",
    [LABEL_MENU + MENU_RC_BASS_DOWN]    = "Descente basses",
    [LABEL_MENU + MENU_RC_MIDDLE_UP]    = "Montée médiums",
    [LABEL_MENU + MENU_RC_MIDDLE_DOWN]  = "Descente médiums",
    [LABEL_MENU + MENU_RC_TREBLE_UP]    = "Montée aigus",
    [LABEL_MENU + MENU_RC_TREBLE_DOWN]  = "Descente aigus",
    [LABEL_MENU + MENU_RC_LOUDNESS]     = "Loudness",
    [LABEL_MENU + MENU_RC_SURROUND]     = "Surround",
    [LABEL_MENU + MENU_RC_EFFECT_3D]    = "Effet 3D",
    [LABEL_MENU + MENU_RC_TONE_BYPASS]  = "Pas de ton",
    [LABEL_MENU + MENU_RC_TIME]         = "Temps",
    [LABEL_MENU + MENU_RC_STOP]         = "Stop",
    [LABEL_MENU + MENU_RC_PLAY_PAUSE]   = "Jouer/pause",
    [LABEL_MENU + MENU_RC_REW]          = "Rembobiner",
    [LABEL_MENU + MENU_RC_FWD]          = "Réinitialisation mémoire",
    [LABEL_MENU + MENU_RC_TIMER]        = "Minuteur",
//    [LABEL_MENU + MENU_RC_SP_MODE]      = "Spectrum mode",
//    [LABEL_MENU + MENU_RC_SCR_DEF]      = "Default screen",
};

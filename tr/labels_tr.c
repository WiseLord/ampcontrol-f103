#include "labels.h"

const char *const labels_tr[LABEL_END] = {
    [LABEL_SUNDAY]          = "PAZAR",
    [LABEL_MONDAY]          = "PAZARTESİ",
    [LABEL_TUESDAY]         = "SALI",
    [LABEL_WEDNESDAY]       = "ÇARŞAMBA",
    [LABEL_THURSDAY]        = "PERŞEMBE",
    [LABEL_FRIDAY]          = "CUMA",
    [LABEL_SATURDAY]        = "CUMARTESİ",

    [LABEL_BRIGNTNESS]      = "Parlaklık",
    [LABEL_MENU + MENU_SETUP_MUTESTBY]  = "Mute & Bekl.. pini",

    [LABEL_VOLUME]          = "Volum",
    [LABEL_BASS]            = "Bass",
    [LABEL_MIDDLE]          = "Middle",
    [LABEL_TREBLE]          = "Tiz",
    [LABEL_FRONTREAR]       = "Ön & Arka",
    [LABEL_BALANCE]         = "Denge",
    [LABEL_CENTER]          = "Merkez",
    [LABEL_SUBWOOFER]       = "Subwoofer",
    [LABEL_PREAMP]          = "Kazanç",

    [LABEL_IN_TUNER]        = "Radio",
    [LABEL_IN_PC]           = "Bilgisayar",
    [LABEL_IN_TV]           = "TV Seti",
//    [LABEL_IN_BLUETOOTH]    = "Bluetooth",
    [LABEL_IN_DVD]          = "DVD Seti",
//    [LABEL_IN_USB]          = "USB",
    [LABEL_IN_MICROPHONE]   = "Mikrofon",
    [LABEL_IN_GUITAR]       = "Gitar",
    [LABEL_IN_TURNTABLES]   = "Plak Çalar",
    [LABEL_IN_CASSETTE]     = "Kaset Çalar",
    [LABEL_IN_PROJECTOR]    = "Projeksiyon",
    [LABEL_IN_SATELLITE]    = "Uydu Alıcısı",
    [LABEL_IN_MIXER]        = "Mikser",

    [LABEL_BOOL_OFF]        = "Kapalı",
    [LABEL_BOOL_ON]         = "Açık",

    [LABEL_TUNER_IC + TUNER_IC_NO]      = "Yok",

    [LABEL_TUNER_BAND + TUNER_BAND_FM_US_EUROPE]    = "ABD/Avrupa FM",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_JAPAN]        = "Japan FM",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_WORLDWIDE]    = "Dünya Çapında FM",
    [LABEL_TUNER_BAND + TUNER_BAND_FM_EASTEUROPE]   = "Doğu Avrupa FM",

//    [LABEL_TUNER_STEP + TUNER_STEP_50K]     = "50kHz",
//    [LABEL_TUNER_STEP + TUNER_STEP_100K]    = "100kHz",
//    [LABEL_TUNER_STEP + TUNER_STEP_200K]    = "200kHz",

//    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_50u] = "50us",
//    [LABEL_TUNER_DEEMPH + TUNER_DEEMPH_75u] = "75us",

    [LABEL_TUNER_MODE + TUNER_MODE_GRID]        = "Izgara",
    [LABEL_TUNER_MODE + TUNER_MODE_STATIONS]    = "İstasyonlar",
    [LABEL_TUNER_MODE + TUNER_MODE_SCAN]        = "Tarama",

    [LABEL_SPECTRUM_MODE + SP_MODE_STEREO]      = "Stereo",
    [LABEL_SPECTRUM_MODE + SP_MODE_MIXED]       = "Karışık",
    [LABEL_SPECTRUM_MODE + SP_MODE_WATERFALL]   = "Şelale",

    [LABEL_MENU + AUDIO_IC_NO]            = "Ses İşlemci Modeli",

    [LABEL_MENU + MENU_NULL]            = "Bir Önceki Menü",

    [LABEL_MENU + MENU_SETUP]           = "Menü Ayarları",
    [LABEL_MENU + MENU_SETUP_LANG]      = "Dil Seçeneği",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Ses İşlemcisi",
    [LABEL_MENU + MENU_SETUP_TUNER]     = "Radyo İşlemcisi",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Spectrum Ayarı",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Ekran Ayarı",
    [LABEL_MENU + MENU_SETUP_INPUT]     = "Giriş Ayarları",
    [LABEL_MENU + MENU_SETUP_RC]        = "Kumanda Ayarı",

    [LABEL_MENU + MENU_AUDIO_IC]        = "Ses İşlemci Modeli",

    [LABEL_MENU + MENU_AUDIO_IN_0]      = "Giriş 0",
    [LABEL_MENU + MENU_AUDIO_IN_1]      = "Giriş 1",
    [LABEL_MENU + MENU_AUDIO_IN_2]      = "Giriş 2",
    [LABEL_MENU + MENU_AUDIO_IN_3]      = "Giriş 3",
    [LABEL_MENU + MENU_AUDIO_IN_4]      = "Giriş 4",
    [LABEL_MENU + MENU_AUDIO_IN_5]      = "Giriş 5",
    [LABEL_MENU + MENU_AUDIO_IN_6]      = "Giriş 6",
    [LABEL_MENU + MENU_AUDIO_IN_7]      = "Giriş 7",

    [LABEL_MENU + MENU_TUNER_IC]        = "Tuner İşlemci Modeli",
    [LABEL_MENU + MENU_TUNER_BAND]      = "Bant Modeli",
    [LABEL_MENU + MENU_TUNER_STEP]      = "Tarama Adımı",
    [LABEL_MENU + MENU_TUNER_DEEMPH]    = "Vurgulama",
    [LABEL_MENU + MENU_TUNER_MODE]      = "Kontrol modu",
    [LABEL_MENU + MENU_TUNER_FMONO]     = "Mono Ses",
    [LABEL_MENU + MENU_TUNER_RDS]       = "Rds Bilgi Gösterimi",
    [LABEL_MENU + MENU_TUNER_BASS]      = "Bass Kuvetlendirme",
    [LABEL_MENU + MENU_TUNER_VOLUME]    = "Tuner Ses Ayarı",

    [LABEL_MENU + MENU_SPECTURM_MODE]   = "Ekran modu",

    [LABEL_MENU + MENU_DISPLAY_BR_STBY] = "Bekleme Parlaklığı",
    [LABEL_MENU + MENU_DISPLAY_ROTATE]  = "Döndürme",

    [LABEL_MENU + MENU_INPUT_ENC_RES]   = "Enkoder Çözünürlüğü",

    [LABEL_MENU + MENU_RC_STBY_SWITCH]  = "Bekleme Butonu",
    [LABEL_MENU + MENU_RC_MUTE]         = "Mute",
    [LABEL_MENU + MENU_RC_VOL_UP]       = "Volum Artı",
    [LABEL_MENU + MENU_RC_VOL_DOWN]     = "Volum Eksi",
    [LABEL_MENU + MENU_RC_MENU]         = "Menü",
    [LABEL_MENU + MENU_RC_CHAN_NEXT]    = "Sornaki Kanal",
    [LABEL_MENU + MENU_RC_CHAN_PREV]    = "Önceki Kanal",
    [LABEL_MENU + MENU_RC_DIG_0]        = "Buton 0",
    [LABEL_MENU + MENU_RC_DIG_1]        = "Buton 1",
    [LABEL_MENU + MENU_RC_DIG_2]        = "Buton 2",
    [LABEL_MENU + MENU_RC_DIG_3]        = "Buton 3",
    [LABEL_MENU + MENU_RC_DIG_4]        = "Buton 4",
    [LABEL_MENU + MENU_RC_DIG_5]        = "Buton 5",
    [LABEL_MENU + MENU_RC_DIG_6]        = "Buton 6",
    [LABEL_MENU + MENU_RC_DIG_7]        = "Buton 7",
    [LABEL_MENU + MENU_RC_DIG_8]        = "Buton 8",
    [LABEL_MENU + MENU_RC_DIG_9]        = "Buton 9",
    [LABEL_MENU + MENU_RC_IN_NEXT]      = "Sonraki Giriş",
    [LABEL_MENU + MENU_RC_IN_PREV]      = "Önceki Giriş",
    [LABEL_MENU + MENU_RC_IN_0]         = "Giriş 0",
    [LABEL_MENU + MENU_RC_IN_1]         = "Giriş 1",
    [LABEL_MENU + MENU_RC_IN_2]         = "Giriş 2",
    [LABEL_MENU + MENU_RC_IN_3]         = "Giriş 3",
    [LABEL_MENU + MENU_RC_IN_4]         = "Giriş 4",
    [LABEL_MENU + MENU_RC_IN_5]         = "Giriş 5",
    [LABEL_MENU + MENU_RC_IN_6]         = "Giriş 6",
    [LABEL_MENU + MENU_RC_IN_7]         = "Giriş 7",
    [LABEL_MENU + MENU_RC_BASS_UP]      = "Bass Artı",
    [LABEL_MENU + MENU_RC_BASS_DOWN]    = "Bass Eksi",
    [LABEL_MENU + MENU_RC_MIDDLE_UP]    = "Orta Ses Artı",
    [LABEL_MENU + MENU_RC_MIDDLE_DOWN]  = "Orta Ses Eksi",
    [LABEL_MENU + MENU_RC_TREBLE_UP]    = "Tiz Artı",
    [LABEL_MENU + MENU_RC_TREBLE_DOWN]  = "Tiz Eksi",
    [LABEL_MENU + MENU_RC_LOUDNESS]     = "Ses Yükseltme",
    [LABEL_MENU + MENU_RC_SURROUND]     = "Kuşatma",
    [LABEL_MENU + MENU_RC_EFFECT_3D]    = "3D-Efekt",
    [LABEL_MENU + MENU_RC_TONE_BYPASS]  = "Ton Baypas",
    [LABEL_MENU + MENU_RC_TIME]         = "Zaman",
    [LABEL_MENU + MENU_RC_STBY_ENTER]   = "Bekleme Modu Enter",
    [LABEL_MENU + MENU_RC_STBY_EXIT]    = "Bekleme Modu Çıkış",
};

#ifndef ICONS_H
#define ICONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "fonts.h"

typedef uint8_t Icon;
enum {
    ICON_EMPTY = 0x00,

    // Various icons
    ICON_RDS,
    ICON_STEREO,
    ICON_FMONO,

    ICON_REPEAT = 0x10,
    ICON_RANDOM,
    ICON_SINGLE,
    ICON_CONSUME,

    // Audio tune icons
    ICON_VOLUME = 0x20,
    ICON_BASS,
    ICON_MIDDLE,
    ICON_TREBLE,
    ICON_BALANCE,
    ICON_FRONTREAR,
    ICON_CENTER,
    ICON_SUBWOOFER,
    ICON_PREAMP,
    ICON_LOUDNESS,

    // Input icons
    ICON_TUNER = 0x40,
    ICON_PC,
    ICON_AUX,
    ICON_BLUETOOTH,
    ICON_KARADIO,
    ICON_USB,
    ICON_SDCARD,
    ICON_MPD,

    // Flag icons
    ICON_TONE = 0x60,
    ICON_SURROUND,
    ICON_EFFECT_3D,

    // Media keys
    ICON_PREV_TRACK = 0x80,
    ICON_NEXT_TRACK,
    ICON_STOP,
    ICON_PLAY_PAUSE,
    ICON_REWIND,
    ICON_FFWD,
};

extern const tFont iconsamp24;
extern const tFont iconsamp32;
extern const tFont iconsamp48;
extern const tFont iconsamp64;

#ifdef __cplusplus
}
#endif

#endif // ICONS_H

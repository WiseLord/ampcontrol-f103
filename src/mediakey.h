#ifndef MEDIAKEY_H
#define MEDIAKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t MediaKey;
enum {
    MEDIAKEY_NONE       = 0,
    MEDIAKEY_PLAY,
    MEDIAKEY_PAUSE,
    MEDIAKEY_STOP,
    MEDIAKEY_PREV,
    MEDIAKEY_NEXT,
    MEDIAKEY_REWIND,
    MEDIAKEY_FFWD,
    MEDIAKEY_REPEAT,
    MEDIAKEY_RANDOM,
    MEDIAKEY_SINGLE,
    MEDIAKEY_CONSUME,
    MEDIAKEY_EJECT,
    MEDIAKEY_INJECT,
    MEDIAKEY_MUTE,
};

#ifdef __cplusplus
}
#endif

#endif // MEDIAKEY_H

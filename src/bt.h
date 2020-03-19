#ifndef BT_H
#define BT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hidkeys.h"

#define SONG_NAME_SIZE  128

typedef uint8_t BtInput;
enum {
    BT_IN_BLUETOOTH = (1 << 0),
    BT_IN_USB       = (1 << 1),
    BT_IN_SDCARD    = (1 << 2),
};

void btInit(void);

void btSendMediaKey(HidMediaKey cmd);

void btReleaseKey(void);

BtInput btGetInput(void);
void btAddInput(BtInput value);
void btDelInput(BtInput value);
void btSetInput(BtInput value);

void btNextInput(void);

char *btGetSongName(void);

#ifdef __cplusplus
}
#endif

#endif // BT_H

#ifndef BT_H
#define BT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "mediakey.h"

#define SONG_NAME_SIZE          128
#define BT_FLAG_NAME_CHANGED    0x01

typedef uint8_t BtInput;
enum {
    BT_IN_BLUETOOTH = (1 << 0),
    BT_IN_USB       = (1 << 1),
    BT_IN_SDCARD    = (1 << 2),
};

typedef struct {
    char songName[SONG_NAME_SIZE];
    uint16_t flags;
    BtInput inMask;
    BtInput input;
} BTCtx;

void btInit(void);
BTCtx *btCtxGet(void);

void btSendMediaKey(MediaKey key);

void btReleaseKey(void);

BtInput btGetInput(void);
void btAddInput(BtInput value);
void btDelInput(BtInput value);
void btSetInput(BtInput value);

void btNextInput(void);

char *btGetSongName(void);

void bt201ParseMount(char *line);
void bt201ParseInput(char *line);
void bt201ParseSongName(char *line, int16_t size);

uint16_t btGetFlags(void);

#ifdef __cplusplus
}
#endif

#endif // BT_H

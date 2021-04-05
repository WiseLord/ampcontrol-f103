#ifndef MPC_H
#define MPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "mediakey.h"

#define MPC_META_SIZE           128
#define MPC_NAME_SIZE           64
#define IP_STR_SIZE             20

typedef uint16_t MpcFlags;
enum {
    MPC_FLAG_UPDATE_META        = 0x0001,
    MPC_FLAG_UPDATE_ELAPSED     = 0x0002,
    MPC_FLAG_UPDATE_DURATION    = 0x0004,
    MPC_FLAG_UPDATE_STATUS      = 0x0008,
    MPC_FLAG_UPDATE_NAME        = 0x0100,
    MPC_FLAG_UPDATE_TRACKNUM    = 0x0200,
};

typedef uint16_t MpcStatus;
enum {
    MPC_IDLE    = 0x0000,

    MPC_PLAYING = 0x0001,
    MPC_PAUSED  = 0x0002,
    MPC_REPEAT  = 0x0010,
    MPC_SINGLE  = 0x0020,
    MPC_RANDOM  = 0x0040,
    MPC_CONSUME = 0x0080,
    MPC_ONLINE  = 0x0100,
    MPC_BT_ON   = 0x0200,
};

typedef struct {
    MpcFlags flags;
    MpcStatus status;
    int32_t trackNum;
    int32_t elapsed;
    int32_t duration;
    char meta[MPC_META_SIZE];
    char name[MPC_NAME_SIZE];
    char ip[IP_STR_SIZE];
} Mpc;

void mpcInit(void);
Mpc *mpcGet(void);

void mpcSyncRequest(void);

void mpcSendDigit(uint8_t dig);
void mpcFinishDigitInput(void);

void mpcLoadPlaylist(int16_t num);
void mpcPlayTrack(int16_t num);

void mpcSendMediaKey(MediaKey key);

void mpcGetData(void);

void mpcSchedPower(bool value);
void mpcSetBluetooth(bool value);

#ifdef __cplusplus
}
#endif

#endif // MPC_H

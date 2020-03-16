#ifndef BT_H
#define BT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hidkeys.h"

typedef uint8_t BtInput;
enum {
    BT_IN_BLUETOOTH,
    BT_IN_USB,
    BT_IN_SDCARD,
};

void btInit(void);

void btSendMediaKey(HidMediaKey cmd);

void btReleaseKey(void);

BtInput btGetInput(void);
void btSetInput(BtInput value);

#ifdef __cplusplus
}
#endif

#endif // BT_H

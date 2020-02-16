#ifndef BT_H
#define BT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hidkeys.h"

void btInit(void);

void btSendMediaKey(HidMediaKey cmd);

void btReleaseKey(void);

#ifdef __cplusplus
}
#endif

#endif // BT_H

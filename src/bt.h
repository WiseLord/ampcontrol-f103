#ifndef BT_H
#define BT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hidkeys.h"

void btSendMediaKey(HidMediaKey cmd);

#ifdef __cplusplus
}
#endif

#endif // BT_H

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void dbgInit(void);
void dbg(const char *str);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H

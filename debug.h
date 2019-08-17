#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define DBG(fmt, ...)  do { dbgPrintf(fmt, ## __VA_ARGS__); } while(0)

void dbgInit(void);
void dbgPrintf (const char *fmt, ...);
void dbgIRQ(void);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H

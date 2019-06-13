#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG_ENABLED
#define DBG(fmt, ...)  do { dbgPrintf(fmt, ## __VA_ARGS__); } while(0)
#else
#define DBG(fmt, ...)
#endif

void dbgInit(void);
void dbgPrintf (const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H

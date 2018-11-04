#ifndef RC_H
#define RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void rcInit(void);
void rcIRQ(void);
void rcOvfIRQ(void);

bool rcGetValue(void);
uint16_t rcGetOvfCnt(void);

#ifdef __cplusplus
}
#endif

#endif // RC_H

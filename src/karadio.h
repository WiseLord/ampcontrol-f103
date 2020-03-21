#ifndef KARADIO_H
#define KARADIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "hidkeys.h"

#define KARADIO_FLAG_NAME       0x01
#define KARADIO_FLAG_META       0x02

#define KARADIO_FLAG_ALL        (KARADIO_FLAG_NAME | KARADIO_FLAG_META)

#define ST_NUM_SIZE             4
#define ST_NAME_SIZE            40
#define ST_META_SIZE            64

typedef struct {
    char num[ST_NUM_SIZE];
    char name[ST_NAME_SIZE];
    char meta[ST_META_SIZE];
    uint16_t flags;
} KaRadio;

void karadioInit(void);
KaRadio *karadioGet(void);

void karadioSetEnabled(bool value);

void karadioSendMediaKey(HidMediaKey cmd);

void karadioGetData(void);
void karadioIRQ(char data);

#ifdef __cplusplus
}
#endif

#endif // KARADIO_H

#ifndef KARADIO_H
#define KARADIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "hidkeys.h"

#define KARADIO_FLAG_NAME       0x0001
#define KARADIO_FLAG_META       0x0002
#define KARADIO_FLAG_NUMBER     0x0004
#define KARADIO_FLAG_PLAYING    0x0100

#define KARADIO_FLAG_ALL        (KARADIO_FLAG_NAME | KARADIO_FLAG_META | KARADIO_FLAG_NUMBER | KARADIO_FLAG_PLAYING)


#define ST_NUM_SIZE             4
#define ST_NAME_SIZE            40
#define ST_META_SIZE            64

typedef struct {
    char num[ST_NUM_SIZE];
    char name[ST_NAME_SIZE];
    char meta[ST_META_SIZE];
    uint16_t flags;
    int16_t station;
} KaRadio;

void karadioInit(void);
KaRadio *karadioGet(void);

void karadioPlayStation(int16_t num);

void kaRadioSendDigit(uint8_t dig);
void kaRadioFinishDigitInput(void);

void karadioSendMediaKey(HidMediaKey cmd);

void karadioGetData(void);

#ifdef __cplusplus
}
#endif

#endif // KARADIO_H

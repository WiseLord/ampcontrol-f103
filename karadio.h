#ifndef KARADIO_H
#define KARADIO_H

#include <stdint.h>
#include <stdbool.h>

#define KARADIO_FLAG_NAME       0x01
#define KARADIO_FLAG_META       0x02

#define KARADIO_FLAG_ALL        (KARADIO_FLAG_NAME | KARADIO_FLAG_META)

#define ST_NUM_SIZE             4
#define ST_NAME_SIZE            40
#define ST_META_SIZE            64

#define USART_KARADIO           USART2

typedef struct {
    char num[ST_NUM_SIZE];
    char name[ST_NAME_SIZE];
    char meta[ST_META_SIZE];
    uint16_t flags;
} KaRadioData;

void karadioInit(void);

void karadioSetEnabled(bool value);

KaRadioData *karadioGet(void);

void karadioUpdateStatus(void);
void karadioSendMediaCmd(uint8_t cmd);

void karadioGetData(void);
void karadioIRQ(void);

#endif // KARADIO_H

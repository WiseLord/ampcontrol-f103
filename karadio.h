#ifndef KARADIO_H
#define KARADIO_H

#include <stdint.h>
#include <stdbool.h>

#define USART_KARADIO           USART2

#define KARADIO_FLAG_NAME       0x01
#define KARADIO_FLAG_META       0x02

#define KARADIO_FLAG_ALL        (KARADIO_FLAG_NAME | KARADIO_FLAG_META)

typedef struct {
    char *name;
    char *meta;
    uint16_t flags;
    bool playing;
} KaRadioData;

void karadioInit(void);
KaRadioData *karadioGet(void);

void karadioUpdateStatus(void);
void karadioSendMediaCmd(uint8_t cmd);

void karadioIRQ(void);

#endif // KARADIO_H

#ifndef KARADIO_H
#define KARADIO_H

#include "stdint.h"

#define USART_KARADIO       USART2

void karadioInit(void);

void karadioSendMediaCmd(uint8_t cmd);

char *karadioGetName(void);
char *karadioGetMeta(void);

void karadioIRQ(void);

#endif // KARADIO_H

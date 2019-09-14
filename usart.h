#ifndef USART_H
#define USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void usartInit(void *usart, uint32_t baudRate);
void usartSetRxIRQ(void *usart, bool enabled);

void usartSendChar(void *usart, char ch);
void usartSendString(void *usart, char *str);

#ifdef __cplusplus
}
#endif

#endif // USART_H

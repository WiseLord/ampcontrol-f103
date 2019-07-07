#ifndef USART_H
#define USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stm32f1xx.h>

void usartInit(USART_TypeDef *USARTx, uint32_t baudRate);

void usartSendChar(USART_TypeDef *USARTx, char ch);

#ifdef __cplusplus
}
#endif

#endif // USART_H

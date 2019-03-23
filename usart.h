#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stm32f1xx_ll_bus.h>

void usartInit(USART_TypeDef *USARTx, uint32_t baudRate);

void usartSendChar(USART_TypeDef *USARTx, char ch);
void usartSendString(USART_TypeDef *USARTx, char *str);

#endif // USART_H

#include "usart.h"

#include "hwlibs.h"

static void usartInitPins(USART_TypeDef *USARTx)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_TypeDef *gpio = GPIOA;
    uint32_t txPin =  LL_GPIO_PIN_9;
    uint32_t rxPin =  LL_GPIO_PIN_10;

    if (USARTx == USART2) {
        txPin = LL_GPIO_PIN_2;
        rxPin = LL_GPIO_PIN_3;
    } else if (USARTx == USART3) {
        gpio = GPIOB;
        txPin = LL_GPIO_PIN_11;
        rxPin = LL_GPIO_PIN_10;
    }

    GPIO_InitStruct.Pin = txPin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
#endif
    LL_GPIO_Init(gpio, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = rxPin;
#ifdef _STM32F1
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
#endif
    LL_GPIO_Init(gpio, &GPIO_InitStruct);
}


void usartInit(void *usart, uint32_t baudRate)
{
    USART_TypeDef *USARTx = (USART_TypeDef *)usart;

    // Peripheral clock enable and interrupt init
    if (USARTx == USART1) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
        NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(USART1_IRQn);
    } else if (USARTx == USART2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(USART2_IRQn);
    } else if (USARTx == USART3) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(USART3_IRQn);
    }

    usartInitPins(USARTx);

    LL_USART_InitTypeDef USART_InitStruct = {0};

    USART_InitStruct.BaudRate = baudRate;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Init(USARTx, &USART_InitStruct);
#ifdef _STM32F3
    LL_USART_DisableIT_CTS(USARTx);
    LL_USART_DisableOverrunDetect(USARTx);
    LL_USART_DisableDMADeactOnRxErr(USARTx);
#endif
    LL_USART_ConfigAsyncMode(USARTx);
    LL_USART_Enable(USARTx);
}

void usartSetRxIRQ(void *usart, bool enabled)
{
    if (enabled) {
        LL_USART_EnableIT_RXNE(usart);
    } else {
        LL_USART_DisableIT_RXNE(usart);
    }
}

void usartSendChar(void *usart, char ch)
{
    USART_TypeDef *USARTx = (USART_TypeDef *)usart;

    while (!LL_USART_IsActiveFlag_TXE(USARTx)) {
        // TODO: timeout
    }

    LL_USART_TransmitData8(USARTx, ch);
}

void usartSendString(void *usart, char *str)
{
    USART_TypeDef *USARTx = (USART_TypeDef *)usart;

    while (*str) {
        usartSendChar(USARTx, *str++);
    }
}

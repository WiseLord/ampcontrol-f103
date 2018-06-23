#ifndef PINS_H
#define PINS_H

#define CONCAT(x,y)         x ## y

#include "main.h"

#define OUT(x)                  (LL_GPIO_SetPinMode(CONCAT(x, _Port), CONCAT(x, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN(x)                   (LL_GPIO_SetPinMode(CONCAT(x, _Port), CONCAT(x, _Pin), LL_GPIO_MODE_INPUT))
#define SET(x)                  (LL_GPIO_SetOutputPin(CONCAT(x, _Port), CONCAT(x, _Pin)))
#define CLR(x)                  (LL_GPIO_ResetOutputPin(CONCAT(x, _Port), CONCAT(x, _Pin)))
#define READ(x)                 (LL_GPIO_IsInputPinSet(CONCAT(x, _Port), CONCAT(x, _Pin)))

#define DISP_D0_Port            GPIOA
#define DISP_D0_Pin             LL_GPIO_PIN_0
#define DISP_D1_Port            GPIOA
#define DISP_D1_Pin             LL_GPIO_PIN_1
#define DISP_D2_Port            GPIOA
#define DISP_D2_Pin             LL_GPIO_PIN_2
#define DISP_D3_Port            GPIOA
#define DISP_D3_Pin             LL_GPIO_PIN_3
#define DISP_D4_Port            GPIOA
#define DISP_D4_Pin             LL_GPIO_PIN_4
#define DISP_D5_Port            GPIOA
#define DISP_D5_Pin             LL_GPIO_PIN_5
#define DISP_D6_Port            GPIOA
#define DISP_D6_Pin             LL_GPIO_PIN_6
#define DISP_D7_Port            GPIOA
#define DISP_D7_Pin             LL_GPIO_PIN_7

#define DISP_DATA_Port          GPIOA
#define DISP_DATA_Pin           LL_GPIO_PIN_15
#define DISP_RW_Port            GPIOB
#define DISP_RW_Pin             LL_GPIO_PIN_5
#define DISP_STROB_Port         GPIOB
#define DISP_STROB_Pin          LL_GPIO_PIN_3
#define DISP_CTRL1_Port         GPIOB
#define DISP_CTRL1_Pin          LL_GPIO_PIN_10
#define DISP_CTRL2_Port         GPIOB
#define DISP_CTRL2_Pin          LL_GPIO_PIN_11
#define DISP_RESET_Port         GPIOB
#define DISP_RESET_Pin          LL_GPIO_PIN_4
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_13

#define TEST_A_Port             GPIOB
#define TEST_A_Pin              LL_GPIO_PIN_0
#define TEST_B_Port             GPIOB
#define TEST_B_Pin              LL_GPIO_PIN_1

#define KS0108_D0               DISP_D0
#define KS0108_D1               DISP_D1
#define KS0108_D2               DISP_D2
#define KS0108_D3               DISP_D3
#define KS0108_D4               DISP_D4
#define KS0108_D5               DISP_D5
#define KS0108_D6               DISP_D6
#define KS0108_D7               DISP_D7
#define KS0108_DI               DISP_DATA
#define KS0108_RW               DISP_RW
#define KS0108_E                DISP_STROB
#define KS0108_CS1              DISP_CTRL1
#define KS0108_CS2              DISP_CTRL2
#define KS0108_RES              DISP_RESET
#define KS0108_BCKL             DISP_BCKL

#endif // PINS_H

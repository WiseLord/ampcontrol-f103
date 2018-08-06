#ifndef PINS_H
#define PINS_H

#include <stm32f1xx_ll_gpio.h>

void pinsInit(void);
uint8_t pinsGetInput(void);

#define CONCAT(x,y)         x ## y

#define OUT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN_F(p)                 (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_FLOATING))
#define IN_P(p)                 (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_INPUT))
#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define READ(p)                 (LL_GPIO_IsInputPinSet(CONCAT(p, _Port), CONCAT(p, _Pin)))

#define SPEED(p, s)             (LL_GPIO_SetPinSpeed(CONCAT(p, _Port), CONCAT(p, _Pin), s))
#define OTYPE(p, t)             (LL_GPIO_SetPinOutputType(CONCAT(p, _Port), CONCAT(p, _Pin), t))

#define OUT_INIT(p, t, s)   \
    do {                    \
        OUT(p);             \
        OTYPE(p, t);        \
        SPEED(p, s);        \
    } while (0);

#define IN_U(p)             \
    do {                    \
        IN_P(p);            \
        SET(p);             \
    } while (0);

#define IN_D(p)             \
    do {                    \
        IN_P(p);            \
        CLR(p);             \
    } while (0);

#define BTN_0_Port              GPIOA
#define BTN_0_Pin               LL_GPIO_PIN_0
#define BTN_1_Port              GPIOA
#define BTN_1_Pin               LL_GPIO_PIN_1
#define BTN_2_Port              GPIOA
#define BTN_2_Pin               LL_GPIO_PIN_2
#define BTN_3_Port              GPIOA
#define BTN_3_Pin               LL_GPIO_PIN_3
#define BTN_4_Port              GPIOA
#define BTN_4_Pin               LL_GPIO_PIN_4
#define BTN_5_Port              GPIOA
#define BTN_5_Pin               LL_GPIO_PIN_5
#define ENC_A_Port              GPIOA
#define ENC_A_Pin               LL_GPIO_PIN_6
#define ENC_B_Port              GPIOA
#define ENC_B_Pin               LL_GPIO_PIN_7

#define INPUT_Port              GPIOC
#define INPUT_Pin               (LL_GPIO_PIN_0  | LL_GPIO_PIN_1  | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)

#define DISP_RS_Port            GPIOA
#define DISP_RS_Pin             LL_GPIO_PIN_15
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

#define KS0108_D0               BTN_0
#define KS0108_D1               BTN_1
#define KS0108_D2               BTN_2
#define KS0108_D3               BTN_3
#define KS0108_D4               BTN_4
#define KS0108_D5               BTN_5
#define KS0108_D6               ENC_A
#define KS0108_D7               ENC_B
#define KS0108_DI               DISP_RS
#define KS0108_RW               DISP_RW
#define KS0108_E                DISP_STROB
#define KS0108_CS1              DISP_CTRL1
#define KS0108_CS2              DISP_CTRL2
#define KS0108_RES              DISP_RESET
#define KS0108_BCKL             DISP_BCKL

#define ILI9320_DHI_Port        GPIOB
#define ILI9320_DHI_Pin         (LL_GPIO_PIN_8  | LL_GPIO_PIN_9  | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | \
                                 LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15)

#define ILI9320_RS_Port         GPIOC
#define ILI9320_RS_Pin          LL_GPIO_PIN_8
#define ILI9320_CS_Port         GPIOC
#define ILI9320_CS_Pin          LL_GPIO_PIN_9
#define ILI9320_WR_Port         GPIOC
#define ILI9320_WR_Pin          LL_GPIO_PIN_10
#define ILI9320_RD_Port         GPIOC
#define ILI9320_RD_Pin          LL_GPIO_PIN_11
#define ILI9320_LED_Port        GPIOC
#define ILI9320_LED_Pin         LL_GPIO_PIN_12

#endif // PINS_H

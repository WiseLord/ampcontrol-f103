#ifndef PINS_H
#define PINS_H

#include <stm32f1xx_ll_gpio.h>

void pinsInit(void);
uint8_t pinsGetInput(void);

#define CONCAT(x,y)         x ## y

#define OUT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN_F(p)                 (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_FLOATING))
#define IN_P(p)                 (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_INPUT))
#define ALT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_ALTERNATE))
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

#define INPUT_Port              BTN_0_Port
#define INPUT_Pin               (BTN_0_Pin | BTN_1_Pin | BTN_2_Pin | BTN_3_Pin | \
                                 BTN_4_Pin | BTN_5_Pin | ENC_A_Pin | ENC_B_Pin)

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
// Temporarly
#if defined(_ILI9320) || defined(_ILI9341)
#define DISP_BCKL_Port          GPIOB
#define DISP_BCKL_Pin           LL_GPIO_PIN_11
#else
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_13
#endif

#define KS0108_DATA_Port        INPUT_Port
#define KS0108_DATA_Pin         INPUT_Pin
#define KS0108_RS               DISP_RS
#define KS0108_RW               DISP_RW
#define KS0108_E                DISP_STROB
#define KS0108_CS1              DISP_CTRL1
#define KS0108_CS2              DISP_CTRL2
#define KS0108_RST              DISP_RESET
#define KS0108_BCKL             DISP_BCKL

// Data High (15..8) bits of ILI9320 are used botn in 16-bit and 8-bit mode
#define ILI9320_DHI_Port        INPUT_Port
#define ILI9320_DHI_Pin         INPUT_Pin
#define ILI9320_CS              DISP_STROB
#define ILI9320_RS              DISP_RS
#define ILI9320_WR              DISP_RW
#define ILI9320_RD              DISP_CTRL1
#define ILI9320_RESET           DISP_RESET
#define ILI9320_LED             DISP_BCKL

#define ILI9341_CS              DISP_RS
#define ILI9341_RESET           DISP_RESET
#define ILI9341_DC              DISP_CTRL1
#define ILI9341_LED             DISP_BCKL
#define ILI9341_SDI             DISP_RW
#define ILI9341_SCK             DISP_STROB
//#define ILI9341_SD0             DISP_RS

#endif // PINS_H

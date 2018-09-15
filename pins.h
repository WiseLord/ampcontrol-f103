#ifndef PINS_H
#define PINS_H

#include <stm32f1xx_ll_gpio.h>

void pinsInit(void);

#define CONCAT(x,y)             x ## y

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
        SPEED(p, s);        \
        OTYPE(p, t);        \
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

#define ALT_INIT(p, t, s)  \
    do {                    \
        ALT(p);             \
        SPEED(p, s);        \
        OTYPE(p, t);        \
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

#define DISP_CS_Port            GPIOA
#define DISP_CS_Pin             LL_GPIO_PIN_15
#define DISP_SCK_Port           GPIOB
#define DISP_SCK_Pin            LL_GPIO_PIN_3
#define DISP_RD_Port            GPIOB
#define DISP_RD_Pin             LL_GPIO_PIN_4
#define DISP_RW_Port            GPIOB
#define DISP_RW_Pin             LL_GPIO_PIN_5
#define DISP_RS_Port            GPIOB
#define DISP_RS_Pin             LL_GPIO_PIN_10
#define DISP_RST_Port           GPIOB
#define DISP_RST_Pin            LL_GPIO_PIN_11
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_13

#define KS0108_DATA_Port        INPUT_Port
#define KS0108_DATA_Pin         INPUT_Pin
#define KS0108_DI               DISP_CS
#define KS0108_RW               DISP_RW
#define KS0108_E                DISP_SCK
#define KS0108_CS1              DISP_RS
#define KS0108_CS2              DISP_RD
#define KS0108_RST              DISP_RST
#define KS0108_BCKL             DISP_BCKL

#define ST7920_DATA_Port        INPUT_Port
#define ST7920_DATA_Pin         INPUT_Pin
#define ST7920_RS               DISP_CS
#define ST7920_RW               DISP_RW
#define ST7920_E                DISP_SCK
#define ST7920_PSB              DISP_RS
#define ST7920_RST              DISP_RST
#define ST7920_BCKL             DISP_BCKL

#define DISP_I2C_SCK_Pin        DISP_RS_Pin
#define DISP_I2C_SCK_Port       DISP_RS_Port
#define DISP_I2C_SDA_Pin        DISP_RST_Pin
#define DISP_I2C_SDA_Port       DISP_RST_Port

#define DISP_8BIT_DHI_Port      INPUT_Port
#define DISP_8BIT_DHI_Pin       INPUT_Pin
#define DISP_8BIT_CS            DISP_CS
#define DISP_8BIT_RS            DISP_RS
#define DISP_8BIT_WR            DISP_RW
#define DISP_8BIT_RD            DISP_RD
#define DISP_8BIT_RST           DISP_RST
#define DISP_8BIT_LED           DISP_BCKL

#define DISP_SPI_CS             DISP_CS
#define DISP_SPI_RST            DISP_RST
#define DISP_SPI_DC             DISP_RS
#define DISP_SPI_SDI            DISP_RW
#define DISP_SPI_SCK            DISP_SCK
#define DISP_SPI_LED            DISP_BCKL
#define DISP_SPI_SDO            DISP_RD


#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_8
#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_9

#endif // PINS_H

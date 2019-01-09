#ifndef PINS_H
#define PINS_H

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_exti.h>

void pinsHwReset(void);
void pinsInit(void);
void pinsInitAmpI2c(void);

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


// Periph macroses
#define I2C_LCD         I2C1
#define I2C_AMP         I2C2

// Input pins (buttons and encoder)
#define BTN_0_Pin               LL_GPIO_PIN_0
#define BTN_1_Pin               LL_GPIO_PIN_1
#define BTN_2_Pin               LL_GPIO_PIN_2
#define BTN_3_Pin               LL_GPIO_PIN_3
#define BTN_4_Pin               LL_GPIO_PIN_4
#define BTN_5_Pin               LL_GPIO_PIN_5
#define ENC_A_Pin               LL_GPIO_PIN_6
#define ENC_B_Pin               LL_GPIO_PIN_7

#define INPUT_Port              GPIOA
#define INPUT_Pin               (BTN_0_Pin | BTN_1_Pin | BTN_2_Pin | BTN_3_Pin | \
                                 BTN_4_Pin | BTN_5_Pin | ENC_A_Pin | ENC_B_Pin)

// Remote control pins
#define RC_Port                 GPIOA
#define RC_Pin                  LL_GPIO_PIN_8
#define RC_ExtiLine             LL_EXTI_LINE_8
#define RC_AR_ExtiPort          LL_GPIO_AF_EXTI_PORTA
#define RC_AR_ExtiLine          LL_GPIO_AF_EXTI_LINE8

// TFT LCD pins
#define DISP_DATA_LO_Port       GPIOA
#define DISP_DATA_LO_Pin        (LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)

// TFT parallel (8-bit) wiring
#define DISP_CS_Port            GPIOB
#define DISP_CS_Pin             LL_GPIO_PIN_7
#define DISP_WR_Port            GPIOB
#define DISP_WR_Pin             LL_GPIO_PIN_5
#define DISP_RS_Port            GPIOB
#define DISP_RS_Pin             LL_GPIO_PIN_6
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_13

// TFT serial (SPI 3 lines) wiring
#define DISP_SPI_SCK_Port       GPIOB
#define DISP_SPI_SCK_Pin        LL_GPIO_PIN_3
#define DISP_SPI_DC             DISP_RS
#define DISP_SPI_SDI            DISP_WR
#define DISP_SPI_SDO            DISP_RD

// TFT I2C (SSD1306) wiring
#define DISP_I2C_Port           GPIOB
#define DISP_I2C_SCK_Pin        LL_GPIO_PIN_6
#define DISP_I2C_SDA_Pin        LL_GPIO_PIN_7

// KS0108 wiring
#define KS0108_DATA_Port        INPUT_Port
#define KS0108_DATA_Pin         INPUT_Pin
#define KS0108_DI               DISP_RS
#define KS0108_RW               DISP_WR
#define KS0108_E                DISP_SPI_SCK
#define KS0108_CS1              DISP_CS
#define KS0108_CS2_Port         GPIOB
#define KS0108_CS2_Pin          LL_GPIO_PIN_4
#define KS0108_BCKL             DISP_BCKL

// ST7920 wiring
#define ST7920_DATA_Port        INPUT_Port
#define ST7920_DATA_Pin         INPUT_Pin
#define ST7920_RS               DISP_RS
#define ST7920_RW               DISP_WR
#define ST7920_E                DISP_SPI_SCK
#define ST7920_PSB              DISP_CS
#define ST7920_BCKL             DISP_BCKL

// Mute and Standby lines
#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_8
#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_9

// Main I2C bus
#define AMP_I2C_Port            GPIOB
#define AMP_I2C_SCK_Pin         LL_GPIO_PIN_10
#define AMP_I2C_SDA_Pin         LL_GPIO_PIN_11


// SI470X Reset pins
#define SI470X_SCLK_Port        AMP_I2C_Port
#define SI470X_SCLK_Pin         AMP_I2C_SCK_Pin
#define SI470X_SDIO_Port        AMP_I2C_Port
#define SI470X_SDIO_Pin         AMP_I2C_SDA_Pin
// TODO: Use the same RST as display uses
#define SI470X_RST_Port         GPIOA
#define SI470X_RST_Pin          LL_GPIO_PIN_15


#endif // PINS_H

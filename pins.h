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

#if defined(_KS0108B)

#define KS0108_DATA_Port        INPUT_Port
#define KS0108_DATA_Pin         INPUT_Pin
#define KS0108_DI               DISP_CS
#define KS0108_RW               DISP_RW
#define KS0108_E                DISP_SCK
#define KS0108_CS1              DISP_RS
#define KS0108_CS2              DISP_RD
#define KS0108_RST              DISP_RST
#define KS0108_BCKL             DISP_BCKL

#elif defined(_SSD1306)

#define SSD1306_SCK_Pin         DISP_RS_Pin
#define SSD1306_SCK_Port        DISP_RS_Port
#define SSD1306_SDA_Pin         DISP_RST_Pin
#define SSD1306_SDA_Port        DISP_RST_Port

#elif defined(_ILI9320)

// Data High (15..8) bits of ILI9320 are used botn in 16-bit and 8-bit mode
#define ILI9320_DHI_Port        INPUT_Port
#define ILI9320_DHI_Pin         INPUT_Pin
#define ILI9320_CS              DISP_CS
#define ILI9320_RS              DISP_RS
#define ILI9320_WR              DISP_RW
#define ILI9320_RD              DISP_RD
#define ILI9320_RESET           DISP_RST
#define ILI9320_LED             DISP_BCKL

#elif defined(_ILI9341)

#define ILI9341_CS              DISP_CS
#define ILI9341_RESET           DISP_RST
#define ILI9341_DC              DISP_RS
#define ILI9341_SDI             DISP_RW
#define ILI9341_SCK             DISP_SCK
#define ILI9341_LED             DISP_BCKL
#define ILI9341_SD0             DISP_RD

#elif defined(_S6D0139)

// Data High (15..8) bits of ILI9320 are used botn in 16-bit and 8-bit mode
#define S6D0139_DHI_Port        INPUT_Port
#define S6D0139_DHI_Pin         INPUT_Pin
#define S6D0139_CS              DISP_CS
#define S6D0139_RS              DISP_RS
#define S6D0139_WR              DISP_RW
#define S6D0139_RD              DISP_RD
#define S6D0139_RESET           DISP_RST
#define S6D0139_LED             DISP_BCKL

#else
#error "Unsupported display driver"
#endif

#endif // PINS_H

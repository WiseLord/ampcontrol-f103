#ifndef PINS_H
#define PINS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "hwlibs.h"

// Mute and Standby lines
#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_11
#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_10

// Main I2C bus
#define AMP_I2C_Port            GPIOB
#define AMP_I2C_SCK_Pin         LL_GPIO_PIN_8
#define AMP_I2C_SDA_Pin         LL_GPIO_PIN_9

// SI470X Reset pins
#define SI470X_SCLK_Port        AMP_I2C_Port
#define SI470X_SCLK_Pin         AMP_I2C_SCK_Pin
#define SI470X_SDIO_Port        AMP_I2C_Port
#define SI470X_SDIO_Pin         AMP_I2C_SDA_Pin
// TODO: Use the same RST as display uses
#define SI470X_RST_Port         GPIOA
#define SI470X_RST_Pin          LL_GPIO_PIN_15

// USB pins
#define USB_DM_Port             GPIOA
#define USB_DM_Pin              LL_GPIO_PIN_11
#define USB_DP_Port             GPIOA
#define USB_DP_Pin              LL_GPIO_PIN_12

#define VOL_UP_Port             GPIOA
#define VOL_UP_Pin              LL_GPIO_PIN_13
#define VOL_DN_Port             GPIOA
#define VOL_DN_Pin              LL_GPIO_PIN_14
#define LOUDNESS_Port           GPIOA
#define LOUDNESS_Pin            LL_GPIO_PIN_15

#define IN_0_Port             GPIOA
#define IN_0_Pin              LL_GPIO_PIN_4
#define IN_1_Port             GPIOA
#define IN_1_Pin              LL_GPIO_PIN_5
#define IN_2_Port             GPIOA
#define IN_2_Pin              LL_GPIO_PIN_6
#define IN_3_Port             GPIOA
#define IN_3_Pin              LL_GPIO_PIN_7

void pinsInit(void);

void pinsHwResetI2c(void);

#ifdef __cplusplus
}
#endif

#endif // PINS_H

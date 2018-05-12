#ifndef PINS_H
#define PINS_H

#define CONCAT(x,y)         x ## y

#include "main.h"

/*
#define DDR(x)              CONCAT(DDR,x)
#define PORT(x)             CONCAT(PORT,x)
#define PIN(x)              CONCAT(PIN,x)

#define OUT(x)              (DDR(x) |= x ## _LINE)
#define IN(x)               (DDR(x) &= ~x ## _LINE)
#define SET(x)              (PORT(x) |= x ## _LINE)
#define CLR(x)              (PORT(x) &= ~x ## _LINE)
#define READ(x)             (PIN(x) & x ## _LINE)
*/


#define SET(x)              (LL_GPIO_SetOutputPin(x ## _GPIO_Port, x ##_Pin))
#define CLR(x)              (LL_GPIO_ResetOutputPin(x ## _GPIO_Port, x ##_Pin))


// KS0108 Data port
#define KS0108_D0_GPIO_Port     DISP_D0_GPIO_Port
#define KS0108_D1_GPIO_Port     DISP_D1_GPIO_Port
#define KS0108_D2_GPIO_Port     DISP_D2_GPIO_Port
#define KS0108_D3_GPIO_Port     DISP_D3_GPIO_Port
#define KS0108_D4_GPIO_Port     DISP_D4_GPIO_Port
#define KS0108_D5_GPIO_Port     DISP_D5_GPIO_Port
#define KS0108_D6_GPIO_Port     DISP_D6_GPIO_Port
#define KS0108_D7_GPIO_Port     DISP_D7_GPIO_Port
#define KS0108_DI_GPIO_Port     DISP_DATA_GPIO_Port
#define KS0108_RW_GPIO_Port     DISP_RW_GPIO_Port
#define KS0108_E_GPIO_Port      DISP_STROB_GPIO_Port
#define KS0108_CS1_GPIO_Port    DISP_CTRL1_GPIO_Port
#define KS0108_CS2_GPIO_Port    DISP_CTRL2_GPIO_Port
#define KS0108_RES_GPIO_Port    DISP_RESET_GPIO_Port
#define KS0108_BCKL_GPIO_Port   DISP_BCKL_GPIO_Port

#define KS0108_D0_Pin           DISP_D0_Pin
#define KS0108_D1_Pin           DISP_D1_Pin
#define KS0108_D2_Pin           DISP_D2_Pin
#define KS0108_D3_Pin           DISP_D3_Pin
#define KS0108_D4_Pin           DISP_D4_Pin
#define KS0108_D5_Pin           DISP_D5_Pin
#define KS0108_D6_Pin           DISP_D6_Pin
#define KS0108_D7_Pin           DISP_D7_Pin
#define KS0108_DI_Pin           DISP_DATA_Pin
#define KS0108_RW_Pin           DISP_RW_Pin
#define KS0108_E_Pin            DISP_STROB_Pin
#define KS0108_CS1_Pin          DISP_CTRL1_Pin
#define KS0108_CS2_Pin          DISP_CTRL2_Pin
#define KS0108_RES_Pin          DISP_RESET_Pin
#define KS0108_BCKL_Pin         DISP_BCKL_Pin


#endif // PINS_H

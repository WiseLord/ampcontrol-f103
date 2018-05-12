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

#define SET(x)              (LL_GPIO_SetOutputPin(CONCAT(x, _GPIO_Port), CONCAT(x, _Pin)))
#define CLR(x)              (LL_GPIO_ResetOutputPin(CONCAT(x, _GPIO_Port), CONCAT(x, _Pin)))

#define KS0108_D0           DISP_D0
#define KS0108_D1           DISP_D1
#define KS0108_D2           DISP_D2
#define KS0108_D3           DISP_D3
#define KS0108_D4           DISP_D4
#define KS0108_D5           DISP_D5
#define KS0108_D6           DISP_D6
#define KS0108_D7           DISP_D7
#define KS0108_DI           DISP_DATA
#define KS0108_RW           DISP_RW
#define KS0108_E            DISP_STROB
#define KS0108_CS1          DISP_CTRL1
#define KS0108_CS2          DISP_CTRL2
#define KS0108_RES          DISP_RESET
#define KS0108_BCKL         DISP_BCKL

#endif // PINS_H

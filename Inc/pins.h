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


#endif // PINS_H

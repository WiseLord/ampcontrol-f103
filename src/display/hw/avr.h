#ifndef AVR_H
#define AVR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#define CONCAT(x,y)             x ## y
#define PCAT(x)                 CONCAT(x, _Port)

#define DDR(x)                  CONCAT(DDR,x)
#define PORT(x)                 CONCAT(PORT,x)
#define PIN(x)                  CONCAT(PIN,x)

#define SET(x)                  PORT(PCAT(x)) |= (1<<x ## _Pin)
#define CLR(x)                  PORT(PCAT(x)) &= ~(1<<x ## _Pin)
#define READ(x)                 PIN(PCAT(x)) & (1<<x ## _Pin)
#define OUT(x)                  DDR(PCAT(x)) |= (1<<x ## _Pin)
#define IN(x)                   DDR(PCAT(x)) &= ~(1<<x ## _Pin)

#define READ_BYTE(p)            (PIN(PCAT(p)))
#define WRITE_BYTE(p, data)     (PORT(PCAT(p)) = data)
#define IN_BYTE(p)              (DDR(PCAT(p)) = 0x00)
#define OUT_BYTE(p)             (DDR(PCAT(p)) = 0xFF)

#ifdef __cplusplus
}
#endif

#endif // AVR_H

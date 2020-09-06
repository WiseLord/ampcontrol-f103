#ifndef STM32F3_H
#define STM32F3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx.h"

#define CONCAT(x,y)             x ## y

#define IS_GPIO_HI(x)           (CONCAT(x, _Pin) & 0xFF00U)
#define IS_GPIO_LO(x)           (CONCAT(x, _Pin) & 0x00FFU)

#define SET(p)                  (CONCAT(p, _Port)->BSRR = CONCAT(p, _Pin))
#define CLR(p)                  (CONCAT(p, _Port)->BRR = CONCAT(p, _Pin))

#define READ(p)                 (CONCAT(p, _Port)->IDR & (CONCAT(p, _Pin)))

#define OUT(p) \
    do { \
        MODIFY_REG(CONCAT(p, _Port)->MODER, \
            (GPIO_MODER_MODER0 << (POSITION_VAL(CONCAT(p, _Pin)) * 2U)), \
            (GPIO_MODER_MODER0_0 << (POSITION_VAL(CONCAT(p, _Pin)) * 2U))); \
        MODIFY_REG(CONCAT(p, _Port)->OSPEEDR, \
            (GPIO_OSPEEDER_OSPEEDR0 << (POSITION_VAL(CONCAT(p, _Pin)) * 2U)), \
            (GPIO_OSPEEDER_OSPEEDR0 << (POSITION_VAL(CONCAT(p, _Pin)) * 2U))); \
        MODIFY_REG(CONCAT(p, _Port)->OTYPER, \
            CONCAT(p, _Pin), \
            0); \
        MODIFY_REG(CONCAT(p, _Port)->PUPDR, \
            (GPIO_PUPDR_PUPDR0 << (POSITION_VAL(CONCAT(p, _Pin)) * 2U)), \
            0); \
    } while (0);

#define READ_BYTE(p)            (IS_GPIO_LO(p) ? \
                                    (READ(p) & 0x00FF) : \
                                    (READ(p) & 0xFF00) >> 8)

#define WRITE_BYTE(p, data)     (CONCAT(p, _Port)->BSRR = (IS_GPIO_LO(p) ? \
                                    (0x00FF0000U | (uint32_t)data) : \
                                    (0xFF000000U | (uint32_t)(data << 8))))

#define IN_BYTE(p) \
    if (IS_GPIO_LO(p)) { \
        CONCAT(p, _Port)->MODER &= ~(0x0000FFFF); \
    } else { \
        CONCAT(p, _Port)->MODER &= ~(0xFFFF0000); \
    } \

#define OUT_BYTE(p) \
    if (IS_GPIO_LO(p)) { \
        CONCAT(p, _Port)->MODER &= ~(0x0000FFFF); \
        CONCAT(p, _Port)->MODER |=  (0x00005555); \
    } else { \
        CONCAT(p, _Port)->MODER &= ~(0xFFFF0000); \
        CONCAT(p, _Port)->MODER |=  (0x55550000); \
    } \

#ifdef __cplusplus
}
#endif

#endif // STM32F3_H

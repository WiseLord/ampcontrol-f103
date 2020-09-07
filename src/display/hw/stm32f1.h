#ifndef STM32F1_H
#define STM32F1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"

#define CONCAT(x,y)             x ## y
#define GPIO(x)                 CONCAT(GPIO, x)

#define IS_GPIO_HI(x)           (CONCAT(x, _Pin) == 0xFF00U)
#define IS_GPIO_LO(x)           (CONCAT(x, _Pin) == 0x00FFU)

#define SET(p)                  GPIO(CONCAT(p, _Port))->BSRR = (1<<CONCAT(p, _Pin))
#define CLR(p)                  GPIO(CONCAT(p, _Port))->BRR = (1<<CONCAT(p, _Pin))

#define READ(p)                 (GPIO(CONCAT(p, _Port))->IDR & (CONCAT(p, _Pin)))

#define OUT(p) \
    if (CONCAT(p, _Pin) < 8) { \
        MODIFY_REG(GPIO(CONCAT(p, _Port))->CRL, \
            ((GPIO_CRL_CNF0 | GPIO_CRL_MODE0) << ((CONCAT(p, _Pin) & 0x7) * 4U)), \
            (0x3 << ((CONCAT(p, _Pin) & 0x7) * 4U))); \
    } else { \
        MODIFY_REG(GPIO(CONCAT(p, _Port))->CRH, \
            ((GPIO_CRL_CNF0 | GPIO_CRL_MODE0) << ((CONCAT(p, _Pin) & 0x7) * 4U)), \
            (0x3 << ((CONCAT(p, _Pin) & 0x7) * 4U))); \
    }

#define READ_BYTE(p)            (IS_GPIO_LO(p) ? \
                                    (READ(p) & 0x00FF) : \
                                    (READ(p) & 0xFF00) >> 8)

#define WRITE_BYTE(p, data)     (GPIO(CONCAT(p, _Port))->BSRR = (IS_GPIO_LO(p) ? \
                                    (0x00FF0000U | (uint32_t)data) : \
                                    (0xFF000000U | (uint32_t)(data << 8))))

#define IN_BYTE(p) \
    if (IS_GPIO_LO(p)) { \
        GPIO(CONCAT(p, _Port))->CRL = 0x88888888; \
    } else { \
        GPIO(CONCAT(p, _Port))->CRH = 0x88888888; \
    } \

#define OUT_BYTE(p) \
    if (IS_GPIO_LO(p)) { \
        GPIO(CONCAT(p, _Port))->CRL = 0x33333333; \
    } else { \
        GPIO(CONCAT(p, _Port))->CRH = 0x33333333; \
    } \

#ifdef __cplusplus
}
#endif

#endif // STM32F1_H

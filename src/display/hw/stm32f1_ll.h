#ifndef HW_STM32F1_LL_H
#define HW_STM32F1_LL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f1xx_ll_gpio.h>

#define CONCAT(x,y)             x ## y

#define OUT_PIN(p) \
    do { \
        LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT); \
        LL_GPIO_SetPinSpeed(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_SPEED_FREQ_HIGH); \
        LL_GPIO_SetPinOutputType(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_OUTPUT_PUSHPULL); \
    } while (0);

#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define TOG(p)                  (LL_GPIO_TogglePin(CONCAT(p, _Port), CONCAT(p, _Pin)))

#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)

#define OUT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN(p)                   (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_INPUT))

#define IS_GPIO_HI(x)           (CONCAT(x, _Pin) & 0x00FF0000U)
#define IS_GPIO_LO(x)           (CONCAT(x, _Pin) & 0x0000FF00U)

#define READ_BYTE(p)            (IS_GPIO_LO(p) ? \
                                    (READ(p) & 0x00FF) : \
                                    (READ(p) & 0xFF00) >> 8)

#define WRITE_BYTE(p, data)     (CONCAT(p, _Port)->BSRR = (IS_GPIO_LO(p) ? \
                                    (0x00FF0000U | (uint32_t)data) : \
                                    (0xFF000000U | (uint32_t)(data << 8))))

#define IN_BYTE(p) \
    if (IS_GPIO_LO(p)) { \
        CONCAT(p, _Port)->CRL = 0x88888888; \
    } else { \
        CONCAT(p, _Port)->CRH = 0x88888888; \
    } \

#define OUT_BYTE(p) \
    if (IS_GPIO_LO(p)) { \
        CONCAT(p, _Port)->CRL = 0x33333333; \
    } else { \
        CONCAT(p, _Port)->CRH = 0x33333333; \
    } \

#ifdef __cplusplus
}
#endif

#endif // HW_STM32F1_LL_H

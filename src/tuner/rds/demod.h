#ifndef RDS_DEMOD_H
#define RDS_DEMOD_H

#ifdef __cplusplus
extern "C" {
#endif

// Remote control pins
#define QUAL_Port               GPIOA
#define QUAL_Pin                LL_GPIO_PIN_4
#define RDDA_Port               GPIOA
#define RDDA_Pin                LL_GPIO_PIN_3
#define RDCL_Port               GPIOA
#define RDCL_Pin                LL_GPIO_PIN_2
#define RDCL_ExtiLine           LL_EXTI_LINE_2
#define RDCL_AR_ExtiPort        LL_GPIO_AF_EXTI_PORTA
#define RDCL_AR_ExtiLine        LL_GPIO_AF_EXTI_LINE2

void rdsDemodInit(void);
void rdsDemodHandle(void);

#ifdef __cplusplus
}
#endif

#endif // DEMOD_H

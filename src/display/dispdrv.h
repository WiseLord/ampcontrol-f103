#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F103xB)
#include "hw/stm32f1.h"
#elif defined (STM32F303xC)
#include "hw/stm32f3.h"
#elif defined (__AVR__)
#include "hw/avr.h"
#endif

#include "dispconf.h"

#include <stdbool.h>
#include <stdint.h>

#include "colors.h"
#include "fonts.h"

#ifdef _DISP_PIXFMT_18BIT
#define TFT_PIXFMT 0x66
#else
#define TFT_PIXFMT 0x55
#endif

typedef struct _DispDriver {
    void (*init)(void);
    void (*sleep)(bool value);
    void (*setIdle)(bool value);
    void (*setWindow)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*rotate)(bool value);
    void (*shift)(int16_t value);
    void (*setBrightness)(uint8_t value);

    void (*fbSync)(void);
    void (*fbSetPixel)(int16_t x, int16_t y, color_t data);
    void (*scanIRQ)(void);

    int16_t width;
    int16_t height;
} DispDriver;

extern const DispDriver dispdrv;

void dispdrvReset(void);
void dispdrvInit(void);

void dispdrvSync(void);
void dispdrvScanIRQ(void);

uint8_t dispdrvGetBus(void);

void dispdrvSendData8(uint8_t data);
void dispdrvSendData16(uint16_t data);

void dispdrvSelectReg8(uint8_t reg);
void dispdrvWriteReg8(uint8_t reg, uint8_t data);

void dispdrvSelectReg16(uint16_t reg);
void dispdrvWriteReg16(uint16_t reg, uint16_t data);

uint16_t dispdrvReadData16(void);
void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs);

void dispdrvDrawPixel(int16_t x, int16_t y, color_t color);
void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color);

void dispdrvDrawVertGrad(int16_t x, int16_t y, int16_t w, int16_t h, color_t *gr);

void dispdrvDrawImage(uint8_t *imgData, int16_t imgWidth,
                      bool portrate, int16_t x, int16_t y,
                      color_t color, color_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H

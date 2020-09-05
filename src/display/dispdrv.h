#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dispconf.h"

#include <stdint.h>

#include "colors.h"
#include "fonts.h"

typedef struct {
    void (*init)(void);
    void (*sleep)(bool value);
    void (*setIdle)(bool value);
    void (*setWindow)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*rotate)(bool value);
    void (*shift)(int16_t value);

    void *fb;
    void (*fbSync)(void);
    void (*fbSetPixel)(int16_t x, int16_t y, color_t data);

    int16_t width;
    int16_t height;
} DispDriver;

extern const DispDriver dispdrv;

void dispdrvReset(void);
void dispdrvInit(void);

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

void dispdrvDrawImage(tImage *img, bool portrate, int16_t x, int16_t y,
                      color_t color, color_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H

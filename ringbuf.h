#ifndef RINGBUF_H
#define RINGBUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    char *data;
    uint16_t capacity;
    volatile uint16_t wrPos;
    volatile uint16_t rdPos;
} RingBuf;

void ringBufInit(RingBuf *rb, char *data, uint16_t size);

void ringBufPushChar(RingBuf *rb, char ch);

char ringBufPopChar(RingBuf *rb);

uint16_t ringBufGetSize(RingBuf *rb);

#ifdef __cplusplus
}
#endif

#endif // RINGBUF_H

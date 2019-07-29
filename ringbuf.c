#include "ringbuf.h"

void ringBufInit(RingBuf *rb, char *data, uint16_t size)
{
    rb->data = data;
    rb->capacity = size;
    rb->wrPos = 0;
    rb->rdPos = 0;
}

void ringBufPushChar(RingBuf *rb, char ch)
{
    rb->data[rb->wrPos++] = ch;
    if (rb->wrPos >= rb->capacity) {
        rb->wrPos = 0;
    }
}

char ringBufPopChar(RingBuf *rb)
{
    char ch = rb->data[rb->rdPos++];
    if (rb->rdPos >= rb->capacity) {
        rb->rdPos = 0;
    }

    return ch;
}

uint16_t ringBufGetSize(RingBuf *rb)
{
    uint16_t size = 0;

    uint16_t rdPos = rb->rdPos;
    uint16_t wrPos = rb->wrPos;

    if (rdPos < wrPos) {
        size = wrPos - rdPos;
    } else if (rdPos > wrPos) {
        size = rb->capacity + wrPos - rdPos;
    }

    return  size;
}

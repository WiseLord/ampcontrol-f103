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
    uint16_t wrPos = rb->wrPos + 1;

    if (wrPos >= rb->capacity) {
        wrPos = 0;
    }
    if (wrPos == rb->rdPos) {
        return;
    }

    rb->data[wrPos] = ch;
    rb->wrPos = wrPos;
}

char ringBufPopChar(RingBuf *rb)
{
    if (rb->rdPos == rb->wrPos) {
        return 0;
    }

    uint16_t rdPos = rb->rdPos + 1;

    if (rdPos >= rb->capacity) {
        rdPos = 0;
    }

    char ch = rb->data[rdPos];
    rb->rdPos = rdPos;

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

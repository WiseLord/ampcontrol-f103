#ifndef STARSVIEW_H
#define STARSVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int16_t offset;
    void *stars;
} StarsView;

void starsView(StarsView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // STARSVIEW_H

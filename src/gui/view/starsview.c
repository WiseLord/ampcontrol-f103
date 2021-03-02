#include "starsview.h"

#include <stdlib.h>

#include "display/glcd.h"
#include "fft.h"

#define STAR_NUM    64
#define FRAMES      256

typedef struct {
    uint8_t frameNum;
    uint8_t size;
    int16_t angle;
} Star;

static int16_t getDistance(uint8_t frameNum)
{
    return (FRAMES - frameNum) * (FRAMES - frameNum) / 128;
}

void drawStar(Star *star, color_t color, int16_t offset)
{
    GlcdRect *rect = glcdGetRect();

    int16_t distance = getDistance(star->frameNum);
    distance += rect->h / 16; // Don't show frames in the center

    int16_t x = fft_cos(star->angle) * distance / 32768 + rect->w / 2;
    int16_t y = fft_sin(star->angle) * distance / 32768 + rect->h / 2 + offset;

    if (color == COLOR_WHITE) {
        color = COLOR_GRAY16(star->size);
    }

    glcdDrawPixel(x - 0, y - 0, color);
    glcdDrawPixel(x - 0, y - 1, color);
    glcdDrawPixel(x - 1, y - 0, color);
    glcdDrawPixel(x - 1, y - 1, color);

    if (y > rect->h || y < 0) {
        star->frameNum = 0;
    }

    if (x > rect->w || x < 0) {
        star->frameNum = 0;
    }
}

void starsView(StarsView *this, bool clear)
{
    (void)clear;

    Star *stars = this->stars;

    for (int i = 0; i < STAR_NUM; i++) {
        if (stars[i].frameNum == 0) {
            int16_t frame = rand() % FRAMES;
            stars[i].frameNum = frame * frame / FRAMES;
            stars[i].size = 0;
            if (this->offset <= -32) {
                stars[i].angle = rand() % (N_WAVE / 2); // Only bottom direction
            } else if (this->offset >= 32) {
                stars[i].angle = rand() % (N_WAVE / 2) + (N_WAVE / 2); // only top direction
            } else {
                stars[i].angle = rand() % N_WAVE;
            }
        } else {
            drawStar(&stars[i], COLOR_BLACK, this->offset);
            stars[i].frameNum--;
            if (stars[i].size < 15) {
                stars[i].size++;
            }
        }
        drawStar(&stars[i], COLOR_WHITE, this->offset);
    }
}

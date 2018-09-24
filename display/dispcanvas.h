#ifndef DISPCANVAS_H
#define DISPCANVAS_H

#include "glcd.h"

void gc160x128Init(GlcdDriver *driver);
void gc176x132Init(GlcdDriver *driver);
void gc220x176Init(GlcdDriver *driver);
void gc320x240Init(GlcdDriver *driver);
void gc400x240Init(GlcdDriver *driver);
void gc480x320Init(GlcdDriver *driver);
void gm128x64Init(GlcdDriver *driver);

#endif // DISPCANVAS_H

#include "canvas.h"

// Canvas variants
void gc160x128Init(Canvas **driver);
void gc176x132Init(Canvas **driver);
void gc220x176Init(Canvas **driver);
void gc320x240Init(Canvas **driver);
void gc400x240Init(Canvas **driver);
void gc480x320Init(Canvas **driver);
void gm128x64Init(Canvas **driver);

void canvasInit(Canvas **canvas)
{
#if defined (_KS0108A) || defined(_KS0108B) || defined (_ST7920) || defined (_SSD1306)
    gm128x64Init(canvas);
#elif defined (_ILI9163) || defined (_ST7735)
    gc160x128Init(canvas);
#elif defined (_LS020) || defined (_LPH9157) || defined (_SSD1286A)
    gc176x132Init(canvas);
#elif defined (_HX8340) || defined (_ILI9225)
    gc220x176Init(canvas);
#elif defined (_ILI9320) || defined (_ILI9341) || defined (_S6D0139) || defined (_SPFD5408) || defined (_MC2PA8201)
    gc320x240Init(canvas);
#elif defined (_ILI9327) || defined (_ST7793)
    gc400x240Init(canvas);
#elif defined (_ILI9481) || defined (_R61581)
    gc480x320Init(canvas);
#else
#error "Unsupported display driver"
#endif
    (*canvas)->color = LCD_COLOR_BLACK;
}

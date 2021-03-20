#include "scrolltext.h"

#include "gui/palette.h"

static void calcNameScroll(ScrollText *this, int16_t max_oft)
{
    if (this->oft >= 0 && !this->pause) {
        this->oft = 0;
        this->pause = 20;
        this->flags &= ~SCROLL_RIGHT;
    }

    if (this->oft <= -max_oft && !this->pause) {
        this->oft = -max_oft;
        this->pause = 20;
        this->flags |= SCROLL_RIGHT;
    }

    if (this->pause > 0) {
        this->pause--;
    }

    if (0 == this->pause) {
        if (this->flags & SCROLL_RIGHT) {
            this->oft++;
        } else {
            this->oft--;
        }
    }
}

static void reset(ScrollText *this)
{
    this->oft = 0;
    this->pause = false;
    this->flags = SCROLL_NO_FLAG;
}

void scrollTextDraw(ScrollText *this, bool clear)
{
    if (clear) {
        reset(this);
    }

    const GlcdRect *rect = this->rect;
    const Palette *pal = paletteGet();

    int16_t len = glcdCalcStringLen(this->text);
    int16_t max_oft = len - rect->w;

    glcdSetRect(rect);

    if (clear || this->flags & SCROLL_EVENT) {
        if (max_oft <= 0) {
            glcdDrawRect(len, 0, rect->w - len, rect->h, pal->bg);
            reset(this);
        } else {
            clear = true;
            calcNameScroll(this, max_oft);
        }
    }

    if (clear) {
        glcdSetXY(this->oft, 0);
        glcdWriteString(this->text);
    }

    glcdResetRect();
}

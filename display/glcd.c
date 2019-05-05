#include "glcd.h"

#include <string.h>
#include "../mem.h"

#define STR_BUFSIZE             64

static Glcd glcd;
static char strbuf[STR_BUFSIZE + 1];    // String buffer

static tImage unRleImg = {
    .rle = 0
};

static tImage *glcdUnRleImg(const tImage *img, uint8_t *data)
{
    tImage *ret = &unRleImg;

    ret->width = img->width;
    ret->height = img->height;

    if (img->rle) {
        // Uncompress image to storage
        const uint8_t *inPtr = img->data;
        uint8_t *outPtr = data;

        while (inPtr < img->data + img->size) {
            int8_t size = (int8_t)(*inPtr);
            inPtr++;
            if (size < 0) {
                for (uint8_t i = 0; i < -size; i++) {
                    *outPtr++ = *inPtr++;
                }
            } else if (size > 0) {
                uint8_t data = *inPtr;
                for (uint8_t i = 0; i < size; i++) {
                    *outPtr++ = data;
                }
                inPtr++;
            } else {
                return 0;
            }
        }
        ret->data = data;
        ret->size = (uint16_t)(outPtr - data);
    } else {
        ret->data = img->data;
        ret->size = img->size;
    }

    return ret;
}

static UChar findSymbolCode(const char **string)
{
    UChar code = 0;
    uint8_t curr = 0;

    const char *str = *string;

    while (*str) {
        uint8_t sym = *str++;

        if ((sym & 0xC0) == 0x80) {         // Not first byte
            code <<= 8;
            code |= sym;
            if (curr) {
                curr--;
            }
        } else {
            code = sym;
            if ((sym & 0x80) == 0x00) {         // one-byte symbol
                curr = 0;
            } else if ((sym & 0xE0) == 0xC0) {  // two-byte symbol
                curr = 1;
            } else if ((sym & 0xF0) == 0xE0) {  // three-byte symbol
                curr = 2;
            } else if ((sym & 0xF8) == 0xF0) {  // four-byte symbol
                curr = 3;
            } else {
                curr = 0;
            }
        }
        if (curr)
            continue;

        *string = str;
        return code;
    }

    *string = 0;
    return BLOCK_CHAR;
}

void glcdInit(Glcd **value)
{
    dispdrvInit();
    glcd.drv = &dispdrv;
    *value = &glcd;
}

bool glcdGetRotate(void)
{
    return glcd.rotate;
}

void glcdRotate(uint8_t rotate)
{
    glcd.rotate = rotate;

    if (glcd.drv->rotate) {
        glcd.drv->rotate(rotate ? LCD_ROTATE_180 : LCD_ROTATE_0);
    }
}

void glcdShift(int16_t pos)
{
    if (glcd.drv->shift) {
        glcd.drv->shift(pos);
    }
}

void glcdSetRect(GlcdRect rect)
{
    glcd.rect = rect;
}

GlcdRect glcdGetRect(void)
{
    return glcd.rect;
}

char *glcdPrepareNum(int32_t number, int8_t width, char lead, uint8_t radix)
{
    uint8_t sign = lead;
    int8_t i;
    int32_t num = number;

    if (number < 0 && radix == 10) {
        sign = '-';
        num = -number;
    }

    for (i = 0; i < width; i++)
        strbuf[i] = lead;
    strbuf[width] = '\0';
    i = width - 1;

    while (num > 0 || i == width - 1) {
        uint8_t numdiv = num % radix;
        strbuf[i] = numdiv + 0x30;
        if (numdiv >= 10)
            strbuf[i] += 7;
        i--;
        num /= radix;
    }

    if (i >= 0)
        strbuf[i] = sign;

    return strbuf;
}

uint16_t glcdWriteNum(int32_t number, int8_t width, char lead, uint8_t radix)
{
    char *str = glcdPrepareNum(number, width, lead, radix);

    return glcdWriteString(str);
}

void glcdSetFont(const tFont *font)
{
    glcd.font.tfont = font;
}

void glcdSetFontColor(uint16_t color)
{
    glcd.font.color = color;
}

void glcdSetFontBgColor(uint16_t color)
{
    glcd.font.bgColor = color;
}

void glcdSetFontAlign(uint8_t align)
{
    glcd.font.align = align;
}

void glcdSetXY(int16_t x, int16_t y)
{
    glcd.x = x;
    glcd.y = y;
}

void glcdSetX(int16_t x)
{
    glcd.x = x;
}
void glcdSetY(int16_t y)
{
    glcd.y = y;
}

int16_t glcdFontSymbolPos(UChar code)
{
    int16_t sPos = -1;

    const tFont *font = glcd.font.tfont;
    for (int16_t i = 0; i < font->length; i++) {
        if (font->chars[i].code == code) {
            return i;
        }
        if (font->chars[i].code == BLOCK_CHAR) {
            sPos = i;
        }
    }

    return sPos;
}

UChar glcdFontSymbolCode(int16_t pos)
{
    const tFont *font = glcd.font.tfont;

    if (pos >= 0 && pos < font->length) {
        return font->chars[pos].code;
    }

    return BLOCK_CHAR;
}

void glcdDrawImage(const tImage *img, uint16_t color, uint16_t bgColor)
{
    if (img == NULL) {
        return;
    }


    int16_t x = glcd.rect.x + glcd.x;
    int16_t y = glcd.rect.y + glcd.y;

    uint8_t *unRleData = mem_malloc(4096);

    tImage *imgUnRle = glcdUnRleImg(img, unRleData);
    dispdrvDrawImage(imgUnRle, x, y, color, bgColor);

    mem_free(unRleData);

    glcdSetX(glcd.x + img->width);
}

const tImage *glcdFindIcon(Icon code, const tFont *iFont)
{
    const tImage *ret = NULL;

    // Find icon pos
    int32_t iPos = -1;
    for (int16_t i = 0; i < iFont->length; i++) {
        if (iFont->chars[i].code == (int32_t)code) {
            iPos = i;
            break;
        }
    }

    if (iPos >= 0) {
        ret = iFont->chars[iPos].image;
    }

    return  ret;
}

uint16_t glcdStrToUStr(const char *str, UChar *ustr)
{
    uint16_t len = 0;

    while (*str) {
        *ustr++ = findSymbolCode(&str);
        len++;
    }

    *ustr = 0;

    return len;
}

void glcdUStrToStr(const UChar *ustr, char *str)
{
    while (*ustr) {
        uint32_t uCode = (uint32_t)(*ustr);
        if ((uCode & 0x00000080) == 0x00000000) { // One-byte symbol
            *str++ = (char)((uCode & 0x000000FF) >> 0);
        } else if ((uCode & 0x0000E0C0) == 0x0000C080) { // Two-byte symbol
            *str++ = (char)((uCode & 0x0000FF00) >> 8);
            *str++ = (char)((uCode & 0x000000FF) >> 0);
        } else if ((uCode & 0x00F0C0C0) == 0x00E08080) { // Three-byte symbol
            *str++ = (char)((uCode & 0x00FF0000) >> 16);
            *str++ = (char)((uCode & 0x0000FF00) >> 8);
            *str++ = (char)((uCode & 0x000000FF) >> 0);
        } else if ((uCode & 0xF8C0C0C0) == 0xF0808080) { // Four-byte symbol
            *str++ = (char)((uCode & 0xFF000000) >> 24);
            *str++ = (char)((uCode & 0x00FF0000) >> 16);
            *str++ = (char)((uCode & 0x0000FF00) >> 8);
            *str++ = (char)((uCode & 0x000000FF) >> 0);
        }
        ustr++;
    }
    *str = 0;
}

int16_t glcdWriteUChar(UChar code)
{
    const tImage *img = NULL;

    int16_t pos = glcdFontSymbolPos(code);

    if (pos < 0)
        return 0;

    img = glcd.font.tfont->chars[pos].image;

    glcdDrawImage(img, glcd.font.color, glcd.font.bgColor);

    return img->width;
}

void glcdSetStringFramed(bool framed)
{
    glcd.strFramed = framed;
}

uint16_t glcdWriteStringConst(const char *string)
{
    strncpy(strbuf, string, STR_BUFSIZE);

    return glcdWriteString(strbuf);
}

uint16_t glcdWriteString(char *string)
{
    UChar code = 0;
    const char *str = string;
    uint16_t ret = 0;

    const tFont *font = glcd.font.tfont;

    if (glcd.font.align != FONT_ALIGN_LEFT) {
        uint16_t strLength = 0;
        int16_t pos = glcdFontSymbolPos(LETTER_SPACE_CHAR);
        int16_t sWidth = font->chars[pos].image->width;

        if (glcd.strFramed)
            strLength += sWidth;
        while (*str) {
            code = findSymbolCode(&str);
            pos = glcdFontSymbolPos(code);
            strLength += font->chars[pos].image->width;
            if (*str) {
                strLength += sWidth;
            }
        }
        if (glcd.strFramed)
            strLength += sWidth;

        if (glcd.font.align == FONT_ALIGN_CENTER) {
            glcdSetX(glcd.x - strLength / 2);
        } else if (glcd.font.align == FONT_ALIGN_RIGHT) {
            glcdSetX(glcd.x - strLength);
        }

        // Reset align after string finished
        glcd.font.align = FONT_ALIGN_LEFT;
    }

    str = string;

    if (glcd.strFramed)
        ret += glcdWriteUChar(LETTER_SPACE_CHAR);
    while (*str) {
        code = findSymbolCode(&str);

        ret += glcdWriteUChar(code);
        if (*str)
            ret += glcdWriteUChar(LETTER_SPACE_CHAR);
    }
    if (glcd.strFramed)
        ret += glcdWriteUChar(LETTER_SPACE_CHAR);

    return ret;
}

void glcdDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    x += glcd.rect.x;
    y += glcd.rect.y;

    dispdrvDrawPixel(x, y, color);
}

void glcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (w < 0 || h < 0)
        return;

    x += glcd.rect.x;
    y += glcd.rect.y;

    dispdrvDrawRectangle(x, y, w, h, color);
}

void glcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    if (x0 == x1) {                 // Vertical
        if (y0 > y1) {              // Swap
            y0 = y0 + y1;
            y1 = y0 - y1;
            y0 = y0 - y1;
        }
        glcdDrawRect(x0, y0, 1, y1 - y0 + 1, color);
    } else if (y0 == y1) {          // Horisontal
        if (x0 > x1) {              // Swap
            x0 = x0 + x1;
            x1 = x0 - x1;
            x0 = x0 - x1;
        }
        glcdDrawRect(x0, y0, x1 - x0 + 1, 1, color);
    } else {
        int16_t sX, sY, dX, dY, err;

        sX = x0 < x1 ? 1 : -1;
        sY = y0 < y1 ? 1 : -1;
        dX = sX > 0 ? x1 - x0 : x0 - x1;
        dY = sY > 0 ? y1 - y0 : y0 - y1;
        err = dX - dY;

        while (x0 != x1 || y0 != y1) {
            glcdDrawPixel(x0, y0, color);
            int16_t err2 = err * 2;
            if (err2 > -dY / 2) {
                err -= dY;
                x0 += sX;
            }
            if (err2 < dX) {
                err += dX;
                y0 += sY;
            }
        }
        glcdDrawPixel(x1, y1, color);
    }
}

void glcdDrawFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, uint16_t color)
{
    glcdDrawRect(x, y, w - t, t, color);
    glcdDrawRect(x, y + t, t, h - t, color);
    glcdDrawRect(x + t, y + h - t, w - t, t, color);
    glcdDrawRect(x + w - t, y, t, h - t, color);
}

void glcdDrawRoundedFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, int16_t r,
                          uint16_t color)
{
    int16_t xc = x + r;
    int16_t yc = y + r;

    int16_t xo = r;
    int16_t xi = xo - t + 1;
    int16_t yo = 0;
    int16_t erro = 1 - xo;
    int16_t erri = 1 - xi;

    while (xo >= yo) {
        glcdDrawLine(xc + xi + w - 2 * r - 1, yc + yo + h - 2 * r - 1, xc + xo + w - 2 * r - 1,
                     yc + yo + h - 2 * r - 1, color);
        glcdDrawLine(xc + yo + w - 2 * r - 1, yc + xi + h - 2 * r - 1, xc + yo + w - 2 * r - 1,
                     yc + xo + h - 2 * r - 1, color);
        glcdDrawLine(xc - xo, yc + yo + h - 2 * r - 1, xc - xi, yc + yo + h - 2 * r - 1, color);
        glcdDrawLine(xc - yo, yc + xi + h - 2 * r - 1, xc - yo, yc + xo + h - 2 * r - 1, color);
        glcdDrawLine(xc - xo, yc - yo, xc - xi, yc - yo, color);
        glcdDrawLine(xc - yo, yc - xo, xc - yo, yc - xi, color);
        glcdDrawLine(xc + xi + w - 2 * r - 1, yc - yo, xc + xo + w - 2 * r - 1, yc - yo, color);
        glcdDrawLine(xc + yo + w - 2 * r - 1, yc - xo, xc + yo + w - 2 * r - 1, yc - xi, color);

        yo++;

        if (erro < 0) {
            erro += 2 * yo + 1;
        } else {
            xo--;
            erro += 2 * (yo - xo + 1);
        }

        if (yo > xo - t + 1) {
            xi = yo;
        } else {
            if (erri < 0) {
                erri += 2 * yo + 1;
            } else {
                xi--;
                erri += 2 * (yo - xi + 1);
            }
        }
    }

    glcdDrawRect(x + r + 1, y, w - 2 * r - 2, t,  color);
    glcdDrawRect(x, y + r + 1, t, h - 2 * r - 2, color);
    glcdDrawRect(x + r + 1, y + h - t, w - 2 * r - 2, t, color);
    glcdDrawRect(x + w - t, y + r + 1, t, h - 2 * r - 2, color);
}

void glcdDrawCircle(int16_t xc, int16_t yc, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    glcdDrawLine(xc - r, yc, xc + r, yc, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        glcdDrawLine(xc - x, yc + y, xc + x, yc + y, color);
        glcdDrawLine(xc - x, yc - y, xc + x, yc - y, color);
        glcdDrawLine(xc - y, yc + x, xc + y, yc + x, color);
        glcdDrawLine(xc - y, yc - x, xc + y, yc - x, color);
    }
}

void glcdDrawRing(int16_t xc, int16_t yc, int16_t r, int16_t t, uint16_t color)
{
    int16_t xo = r;
    int16_t xi = xo - t + 1;
    int16_t y = 0;
    int16_t erro = 1 - xo;
    int16_t erri = 1 - xi;

    while (xo >= y) {
        glcdDrawLine(xc + xi, yc + y, xc + xo, yc + y, color);
        glcdDrawLine(xc + y, yc + xi, xc + y, yc + xo, color);
        glcdDrawLine(xc - xo, yc + y, xc - xi, yc + y, color);
        glcdDrawLine(xc - y, yc + xi, xc - y, yc + xo, color);
        glcdDrawLine(xc - xo, yc - y, xc - xi, yc - y, color);
        glcdDrawLine(xc - y, yc - xo, xc - y, yc - xi, color);
        glcdDrawLine(xc + xi, yc - y, xc + xo, yc - y, color);
        glcdDrawLine(xc + y, yc - xo, xc + y, yc - xi, color);

        y++;

        if (erro < 0) {
            erro += 2 * y + 1;
        } else {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > xo - t + 1) {
            xi = y;
        } else {
            if (erri < 0) {
                erri += 2 * y + 1;
            } else {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}

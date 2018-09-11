#ifndef DISPDRV_H
#define DISPDRV_H

#define DISPDRV_SEND_IMAGE(img, sendData)                       \
    uint16_t color = glcd.font.color;                           \
    uint16_t bgColor = glcd.canvas->color;                      \
    for (uint16_t i = 0; i < w; i++) {                          \
        for (uint16_t j = 0; j < (h + 7) / 8; j++) {            \
            uint8_t data = img->data[w * j + i];                \
            for (uint8_t bit = 0; bit < 8; bit++) {             \
                if (8 * j + bit < h) {                          \
                    sendData(data & 0x01 ? color : bgColor);    \
                    data >>= 1;                                 \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \

#endif // DISPDRV_H

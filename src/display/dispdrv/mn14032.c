#include "../dispdrv.h"

#define DISP_WIDTH           140
#define DISP_HEIGHT          32

#define MN14032_SIN_Port        B   // WR
#define MN14032_SIN_Pin         15
#define MN14032_CLK_Port        B   // SCK
#define MN14032_CLK_Pin         13
#define MN14032_LAT_Port        B   // CS
#define MN14032_LAT_Pin         12
#define MN14032_BLK_Port        B   // RS
#define MN14032_BLK_Pin         14
#define MN14032_GCP_Port        C   // BCKL
#define MN14032_GCP_Pin         13

#define MN14032_COLS            24
#define MN14032_GRID_BITS       6

static uint8_t fb[DISP_HEIGHT * MN14032_COLS];

static void mn14032PinsInit(void)
{
    OUT(MN14032_GCP);
    OUT(MN14032_BLK);
    OUT(MN14032_LAT);
    OUT(MN14032_CLK);
    OUT(MN14032_SIN);
}

void mn14032Init(void)
{
    mn14032PinsInit();

    CLR(MN14032_GCP);
    CLR(MN14032_BLK);
}

static void mn14032SetBit(bool value)
{
    CLR(MN14032_CLK);
    if (value) {
        SET(MN14032_SIN);
    } else {
        CLR(MN14032_SIN);
    }
    __asm volatile ("nop");
    SET(MN14032_CLK);
}


void mn14032Scan()
{
    static uint8_t row = 0;

    CLR(MN14032_LAT);

    uint8_t *pfb = &fb[row / 2];

    // 192 bits of row data
    for (int i = 0; i < DISP_HEIGHT; i++) {
        for (int j = 0; j < MN14032_GRID_BITS; j++) {
            bool bit = !((row + j) % 2) && (*pfb & (1 << j));

            mn14032SetBit(bit);
        }
        pfb += MN14032_COLS;
    }

    // 48 bits of grid row number
    for (int i = 0; i < MN14032_COLS * 2; i++) {
        bool bit = (i == row + 1 || i == row);

        mn14032SetBit(bit);
    }

    SET(MN14032_LAT);

    if (++row + 1 >= MN14032_COLS * 2) {
        row = 0;
    }
}

void mn14032FbSetPixel(int16_t x, int16_t y, color_t color)
{
    static const uint8_t bit[MN14032_GRID_BITS] = {0x01, 0x04, 0x10, 0x20, 0x08, 0x02};

    if (color != 0) {
        fb[x / 6 % 24 + y * 24] |= bit[x % 6];
    } else {
        fb[x / 6 % 24 + y * 24] &= ~bit[x % 6];
    }
}

const DispDriver dispdrv = {
    .width = DISP_WIDTH,
    .height = DISP_HEIGHT,
    .init = mn14032Init,

    .fbSetPixel = mn14032FbSetPixel,
    .scanIRQ = mn14032Scan,
};

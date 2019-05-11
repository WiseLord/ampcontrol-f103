#ifndef RDS_H
#define RDS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t a;
    uint16_t b;
    uint16_t c;
    uint16_t d;
} RdsBlock;

typedef struct {
    uint16_t PI;        // Program identification code

    uint8_t group;
    uint8_t version;
    uint8_t TP;         // Traffic program identification code
    uint8_t PTY;        // Program type code

    bool TA;            // Traffic announcement
    bool MS;            // Music / speach
    bool DI_ST;         // Decoder identification: stereo
    bool DI_AH;
    bool DI_CMP;
    bool DI_stPTY;
    char PS[9];

    char text[65];
} Rds;

void rdsReset(void);
bool rdsGetFlag(void);

void rdsBufToBlock(uint8_t *buf, RdsBlock *block);
void rdsDecode(RdsBlock *block);
Rds *rdsGet(void);

#endif // RDS_H

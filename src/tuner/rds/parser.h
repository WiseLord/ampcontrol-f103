#ifndef RDS_PARSER_H
#define RDS_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef union {
    struct {
        uint16_t a;
        uint16_t b;
        uint16_t c;
        uint16_t d;
    };
    uint16_t blk[4];
} RdsBlock;

typedef uint8_t RDS_Flag;
enum {
    RDS_FLAG_READY      = 0x01,

    RDS_FLAG_TP         = 0x02,
    RDS_FLAG_TA         = 0x04,
    RDS_FLAG_MS         = 0x08,
    RDS_FLAG_DI_ST      = 0x10,
    RDS_FLAG_DI_AH      = 0x20,
    RDS_FLAG_DI_CMP     = 0x40,
    RDS_FLAG_DI_STPTY   = 0x80,
};

typedef struct {
    uint16_t PI;        // Program identification code (16 bits)
    uint8_t PTY;        // Program type code (5 bits)
    RDS_Flag flags;
    char PS[8 + 1];
    char text[64 + 1];
} RdsParser;

typedef void (*RdsParserCb)(void);

void rdsParserReset(void);
void rdsParserSetCb(RdsParserCb cb);

void rdsParserDecode(RdsBlock *block);
RdsParser *rdsParserGet(void);

#ifdef __cplusplus
}
#endif

#endif // RDS_PARSER_H

#ifndef RDS_DECODER_H
#define RDS_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "parser.h"

#define RDS_BLOCK_LEN   26
#define RDS_GROUP_LEN   (RDS_BLOCK_LEN * 4)

typedef struct {
    uint32_t raw;
    RdsBlock block;
    const char *oft_name;
    uint16_t syndrome;
    bool synced;
    int8_t currIdx;
    int8_t currCnt;
    uint8_t goodMask;
    bool corrected;
} RdsDecoder;

void rdsDecoderPushBit(bool data);
RdsDecoder *rdsDecoderGet(void);

#ifdef __cplusplus
}
#endif

#endif // RDS_DECODER_H

#ifndef RC4_H
#define RC4_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


typedef struct {
    uint8_t se[256], sd[256];
    uint32_t pose, posd;
    uint8_t te, td;
} rc4_ctx;

void rc4_ks(rc4_ctx *ctx, const uint8_t *key, uint32_t key_len);
void rc4_encrypt(rc4_ctx *ctx, const uint8_t *src, uint8_t *dst, uint32_t len);
void rc4_decrypt(rc4_ctx *ctx, const uint8_t *src, uint8_t *dst, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif /* !RC4_H */

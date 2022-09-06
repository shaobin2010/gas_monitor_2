#ifndef _RC4_H_
#define _RC4_H_
#include "stdint.h"

/**RC4***/
#define SBOX_LEN    32
#define KEY_LEN     8
#define MKEY_LEN    16
#define DATA_LEN    16

void init_kbox(uint8_t *key, uint8_t keyleng);
void init_sbox(void);
void rc4_encrypt(uint8_t *key, uint8_t key_leng, uint8_t *buf, int len);
void rc4_decrypt(uint8_t *key, uint8_t key_leng, uint8_t *sbuf, uint8_t *dbuf, int len);

#endif

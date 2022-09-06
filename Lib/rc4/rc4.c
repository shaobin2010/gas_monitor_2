#include "rc4.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"


/**RC4***/
uint8_t Rc4Sbox[SBOX_LEN];
uint8_t Rc4Kbox[SBOX_LEN];

void init_sbox(void)
{
    int i, j = 0;
    unsigned char tmp;

    for (i = 0; i < SBOX_LEN; i++)
        Rc4Sbox[i] = i;

    for (i = 0; i < SBOX_LEN; i++)
    {
        j = (j + Rc4Sbox[i] + Rc4Kbox[i]) % SBOX_LEN;
        tmp = Rc4Sbox[i];
        Rc4Sbox[i] = Rc4Sbox[j];
        Rc4Sbox[j] = tmp;
    }
}

/***************************************
 * 功能名: init_kbox
 * 说明:  初始化 kbox
 * 输入:  key 密钥
 * 输出:
 * 返回:
**************************************/
void init_kbox(uint8_t *key, uint8_t keyleng)
{
    int i;
    for (i = 0; i < SBOX_LEN; i++)
    {
        Rc4Kbox[i] = key[i % keyleng];
    }
}

/***************************************
 * 功能名: rc4_encrypt
 * 说明:  初始化 kbox
 * 输入:  key 密钥
 *        key_len
 *        buf
 *        len
 * 输出:
 * 返回:
**************************************/
void rc4_encrypt(uint8_t *key, uint8_t key_leng, uint8_t *buf, int len)
{
    int i = 0, j = 0, t, index = 0;
    uint8_t tmp;
    uint8_t *data;
    data=buf;

    if (buf == NULL || len <= 0)
        return;

    init_kbox(key, key_leng);
    init_sbox();


    while (index < len)
    {
        i = (i + 1) % SBOX_LEN;
        j = (j + Rc4Sbox[i]) % SBOX_LEN;
        tmp = Rc4Sbox[i];
        Rc4Sbox[i] = Rc4Sbox[j];
        Rc4Sbox[j] = tmp;
        t = (Rc4Sbox[i] + Rc4Sbox[j]) % SBOX_LEN;

        buf[index] = data[index] ^ Rc4Sbox[t];
        index++;
    }
}


/***************************************
 * 功能名: rc4_decrypt
 * 说明:  初始化 kbox
 * 输入:  key 密钥
 *        key_len
 *        buf
 *        len
 * 输出:
 * 返回:
**************************************/
void rc4_decrypt(uint8_t *key, uint8_t key_leng, uint8_t *sbuf, uint8_t *dbuf, int len)
{
    int i = 0, j = 0, t, index = 0;
    uint8_t tmp;
//    unsigned char *data;
//    data=dbuf;

    if (sbuf == NULL || len <= 0)
        return;

    init_kbox(key,key_leng);
    init_sbox();


    while (index < len)
    {
        i = (i + 1) % SBOX_LEN;
        j = (j + Rc4Sbox[i]) % SBOX_LEN;
        tmp = Rc4Sbox[i];
        Rc4Sbox[i] = Rc4Sbox[j];
        Rc4Sbox[j] = tmp;
        t = (Rc4Sbox[i] + Rc4Sbox[j]) % SBOX_LEN;

        dbuf[index] = sbuf[index] ^ Rc4Sbox[t];
        index++;
    }
}

#if 0
/*
base64密文
qlUAAGkjIjg2OTk3NTAzODE5NzI5MgIDABFWqAoFGQRe0REQbQs0WwoAC0G4AR8RH1PaFRZrAz5kDC//

base64解密
AA5500006923223836393937353033383139373239320203001156A80A0519045ED111106D0B345B0A000B41B8011F111F53DA15166B033E640C2FFF

拆分数据
AA55
00006923
22
383639393735303338313937323932
02
03001156A80A0519045ED111106D0B345B0A000B41B8011F111F53DA15166B033E64
0C2F
FF


加密部分
03 00 11 56 A8 0A 05 19 04 5E D1 11 10 6D 0B 34 5B 0A 00 0B 41 B8 01 1F 11 1F 53 DA 15 16 6B 03 3E 64



解密后
02 00 00 41 a0 00 00 01 00 41 c8 00 00 63 0f 20 40 02 00 00 41 a0 00 00 01 00 41 c8 00 00 63 0f 20 7c

拆分数据
02
0000
41a00000
0100
41c80000
630f2040
02
0000
41a00000
0100
41c80000
630f207c
 * */

static const uint8_t en_Key[8] = {'3', '0', '4', '0', 'Z', 'H', 'G', 'W'};

uint8_t en_bytes[] = {
		0x03, 0x00, 0x11, 0x56, 0xA8, 0x0A,
		0x05, 0x19, 0x04, 0x5E, 0xD1, 0x11, 0x10,
		0x6D, 0x0B, 0x34, 0x5B, 0x0A, 0x00, 0x0B,
		0x41, 0xB8, 0x01, 0x1F, 0x11, 0x1F, 0x53,
		0xDA, 0x15, 0x16, 0x6B, 0x03, 0x3E, 0x64
};

uint8_t de_bytes[] = {
		0x02, 0x00, 0x00, 0x41, 0xa0, 0x00, 0x00, 0x01, 0x00,
		0x41, 0xc8, 0x00, 0x00, 0x63, 0x0f, 0x20, 0x40, 0x02,
		0x00, 0x00, 0x41, 0xa0, 0x00, 0x00, 0x01, 0x00, 0x41,
		0xc8, 0x00, 0x00, 0x63, 0x0f, 0x20, 0x7c
};

void rc4_test(void)
{
	uint32_t i;
	uint8_t dbuf[200];
	rc4_encrypt(en_Key, 8, de_bytes, sizeof(de_bytes));
	printf("RC4: encrypt:  ");
	for (i = 0; i < sizeof(de_bytes); i++) {
		printf("%02x ", de_bytes[i]);
	}
	printf("\r\n");

	rc4_decrypt(en_Key, 8, en_bytes, dbuf, sizeof(en_bytes));
	printf("RC4: decrypt:  ");
	for (i = 0; i < sizeof(en_bytes); i++) {
		printf("%02x ", dbuf[i]);
	}
	printf("\r\n");
}
#endif

#ifndef __APP_OTA_H__
#define __APP_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define OTA_IMG_FIRST_PAGE_IDX      0
#define OTA_IMG_FIRST_OFFSET        0
#define RAW_IMG_FIRST_OFFSET        16

#define OTA_MAGIC    0x12344321

typedef struct {
	uint32_t magic;       // 0x12344321
	uint32_t img_size;    // OTA image size
	uint32_t crc32;       // crc32 of real image data
	uint32_t rsvd;
} ota_img_header_s;


extern void ota_try_to_upgrade(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_OTA_H__ */

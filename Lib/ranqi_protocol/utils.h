#ifndef __RANQI_UTILS_H__
#define __RANQI_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define U32_LITTLE_BIG(data) (uint32_t)( ((((uint32_t)data) & 0x000000FF) << 24) | \
		                       	   	     ((((uint32_t)data) & 0x0000FF00) << 8)  | \
										 ((((uint32_t)data) & 0x00FF0000) >> 8)  | \
										 ((((uint32_t)data) & 0xFF000000) >> 24))

#define U32_BIG_LITTLE(data)  U32_LITTLE_BIG(data)
#define U16_LITTLE_BIG(data) (uint16_t)( ((((uint16_t)data) & 0x000000FF) << 8) | \
		                       	   	   	 ((((uint16_t)data) & 0x0000FF00) >> 8) )
#define U16_BIG_LITTLE(data)  U16_LITTLE_BIG(data)


extern uint16_t utils_checksum(uint8_t *buff, uint32_t len);
extern float float_little_bit(float value);

#ifdef __cplusplus
}
#endif

#endif /* !__RANQI_UTILS_H__ */

#ifndef __RANQI_UTILS_H__
#define __RANQI_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

extern uint16_t utils_checksum(uint8_t *buff, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* !__RANQI_UTILS_H__ */

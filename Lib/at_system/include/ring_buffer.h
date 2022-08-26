#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"

#define MIN(a, b) (a)<(b)?(a):(b)

typedef struct {
    int32_t   size;
    int32_t   read;
    int32_t   write;
    char     *buff;
    SemaphoreHandle_t rb_Sema;
} ring_buffer_t;


bool ring_buffer_init(ring_buffer_t *rb, void *Buff, int32_t BuffLen);
void ring_buffer_deinit(ring_buffer_t *rb);
int32_t ring_buffer_size(ring_buffer_t *rb);
int32_t ring_buffer_data_size(ring_buffer_t *rb);
int32_t ring_buffer_free_size(ring_buffer_t *rb);
//int32_t ring_buffer_read(ring_buffer_t *rb, void *data, uint32_t count);
int32_t ring_buffer_read(ring_buffer_t *rb, void *data, uint32_t count, uint32_t timeout);
int32_t ring_buffer_write(ring_buffer_t *rb, void *data, uint32_t count);
void ring_buff_flush(ring_buffer_t *rb);


#ifdef __cplusplus
}
#endif
#endif /*__RING_BUFFER_H__ */


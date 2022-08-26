#include <string.h>
#include "include/ring_buffer.h"

bool ring_buffer_init(ring_buffer_t *rb, void *Buff, int32_t BuffLen)
{
    if(NULL == rb) {
		return;
	}

	rb->size = BuffLen;
	rb->buff = Buff;
	rb->read = 0;
	rb->write =0;
    rb->rb_Sema = xSemaphoreCreateBinary();
	if(NULL == rb->rb_Sema) {
		printf("create ringbuff Sema ERROR\r\n");
		return false;
	}
    return true;
}

void ring_buffer_deinit(ring_buffer_t *rb)
{
    if(NULL == rb){
        return;
    }

    rb->buff = NULL;
    rb->read = 0;
    rb->write = 0;
    rb->size = 0;
    vSemaphoreDelete(rb->rb_Sema);
}

int32_t ring_buffer_size(ring_buffer_t *rb)
{
    if(NULL == rb){
        return -1;
    }

    return rb->size;
}

int32_t ring_buffer_data_size(ring_buffer_t *rb)   
{
    if(NULL == rb) {
        return -1;
    }

    if (rb->read == rb->write) { //读与写相遇
        return 0;
    }

    if (rb->read < rb->write)  { //写大于读
        return rb->write - rb->read;
    }

    return rb->size - (rb->read - rb->write); //读大于写
}


int32_t ring_buffer_free_size(ring_buffer_t *rb) //返回能写入的空间
{
    if(NULL == rb) {
        return -1;
    }

    return rb->size - ring_buffer_data_size(rb);  //总的减去已经写入的空间
}

void ring_buff_flush(ring_buffer_t *rb)
{
	rb->read = 0;
	rb->write =0;
}

#if 0
int32_t ring_buffer_read(ring_buffer_t *rb, void *data, uint32_t count)
{
    int32_t copySz = 0;

    if ((NULL == rb) || (NULL == data)) {
        return -1;
    }
    
    copySz = MIN(count, ring_buffer_data_size(rb));  // 查看能读的个数

    if (rb->read < rb->write) {                          // 写大于读    
        memcpy(data, &rb->buff[rb->read], copySz);       // 读出数据到data
        rb->read += copySz;                              // 读指针加上读取的个数
        return copySz;                                   // 返回读取的个数

    } else { //读大于等于写
        if (copySz < rb->size - rb->read)  {               //  读的个数小于头上面的数据量
            memcpy(data, &rb->buff[rb->read], copySz);
            rb->read += copySz;
            return copySz;

        } else {
            int32_t first_part = rb->size - rb->read;            //先读出来头上面的数据
            memcpy(data, &rb->buff[rb->read], first_part);
            rb->read = 0;

            memcpy(&data[first_part], &rb->buff[rb->read], copySz - first_part);
            rb->read += copySz - first_part;

            return copySz;
        }
    }
}

#else 
int32_t ring_buffer_read(ring_buffer_t *rb, void *data, uint32_t count, uint32_t timeout)
{
    int32_t copySz = 0;

    if ((NULL == rb) || (NULL == data)) {
        return -1;
    }

    // No data is available, then wait for the semaphore
    if(ring_buffer_data_size(rb) == 0) {
        if(uxSemaphoreGetCount(rb->rb_Sema) > 0) {
			// clear old Semaphore
			xSemaphoreTake(rb->rb_Sema, 10);
		}

		// Waiting for AT Uart data is available
		if( xSemaphoreTake(rb->rb_Sema, timeout) ==  pdFALSE) { // Timeout
			return -2;
        }
    }
    
    copySz = MIN(count, ring_buffer_data_size(rb));  // 查看能读的个数

    if (rb->read < rb->write) {                          // 写大于读    
        memcpy(data, &rb->buff[rb->read], copySz);       // 读出数据到data
        rb->read += copySz;                              // 读指针加上读取的个数
        return copySz;                                   // 返回读取的个数

    } else { //读大于等于写
        if (copySz < rb->size - rb->read)  {               //  读的个数小于头上面的数据量
            memcpy(data, &rb->buff[rb->read], copySz);
            rb->read += copySz;
            return copySz;

        } else {
            int32_t first_part = rb->size - rb->read;            //先读出来头上面的数据
            memcpy(data, &rb->buff[rb->read], first_part);
            rb->read = 0;

            memcpy(&data[first_part], &rb->buff[rb->read], copySz - first_part);
            rb->read += copySz - first_part;

            return copySz;
        }
    }
}
#endif

int32_t ring_buffer_write(ring_buffer_t *rb, void *data, uint32_t count)
{
    int tailAvailSz = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ((NULL == rb) || (NULL == data)) {
        return -1;
    }

    if (count >= ring_buffer_free_size(rb)) {
        return -1;
    }

    if (rb->read <= rb->write) {
        tailAvailSz = rb->size - rb->write;  // 查看尾上面剩余的空间
        if (count <= tailAvailSz) {
            memcpy(&rb->buff[rb->write], data, count);   // 拷贝数据到环形数组
            rb->write += count;                          // 尾指针加上数据个数
            if (rb->write >= rb->size) {
                rb->write = 0;
            }
        } else {
            memcpy(&rb->buff[rb->write], data, tailAvailSz);
            rb->write = 0;

            memcpy(&rb->buff[rb->write], &data[tailAvailSz], count - tailAvailSz);
            rb->write += count - tailAvailSz;
        }
    } else {
        memcpy(&rb->buff[rb->write], data, count);
        rb->write += count;
        if (rb->write >= rb->size) {
            rb->write = 0;
        }
    }

    xSemaphoreGiveFromISR(rb->rb_Sema, &xHigherPriorityTaskWoken);
    return count;
}

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "ds1302.h"

#define RANQI_QUEUE_DEPTH         2
#define RQNQI_QUEUE_ITEM_SIZE     512

typedef struct
{
   uint16_t ucData_len;
   uint8_t ucData[ RQNQI_QUEUE_ITEM_SIZE ];
} Ranqi_xMessage_s;

extern void board_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__BOARD_H__ */


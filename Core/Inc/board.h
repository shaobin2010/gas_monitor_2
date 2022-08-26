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

typedef enum {
	BOARD_LED_SYS       = 0,
	BOARD_LED_ALM       = 1,
	BOARD_LED_NUM
} board_led_e;

extern void board_init(void);
extern void board_led_on(board_led_e led);
extern void board_led_off(board_led_e led);
extern void board_led_toggle(board_led_e led);

extern uint32_t board_1s_tick_get(void);
extern uint32_t board_1m_tick_get(void);

#ifdef __cplusplus
}
#endif
#endif /*__BOARD_H__ */


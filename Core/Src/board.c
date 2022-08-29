
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rcc.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "Queue.h"

#include "gpio.h"
#include "board.h"


uint8_t unix_tick_set = 0;
uint32_t board_1s_tick = 0;
uint32_t board_1m_tick = 0;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef  htim3;


extern QueueHandle_t Ranqi_Rx_Queue;
static StaticQueue_t xStaticQueue;
uint8_t ucQueueStorageArea[ RANQI_QUEUE_DEPTH * sizeof(Ranqi_xMessage_s) ];

extern void atClient_RxCpltCallback(UART_HandleTypeDef *huart);
extern void gps_RxCpltCallback(UART_HandleTypeDef *huart);

void board_DelayUs(int32_t udelay)
{
  __IO uint32_t Delay = udelay * 72 / 8;
  do {
	  __NOP();
  } while(Delay--);
}


void mem_info(void)
{
	printf("\n\rTotal_mem:%d freeMem:%d  %d\r\n", configTOTAL_HEAP_SIZE,
			 xPortGetFreeHeapSize(),
			 xPortGetMinimumEverFreeHeapSize());
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1) {
		atClient_RxCpltCallback(huart);
	} else if (huart == &huart2) {
		gps_RxCpltCallback(huart);
	} else {

	}
}

void board_set_date_time(ds_time_s *utc_date)
{
	ds_time_s beijing_date;
	if(unix_tick_set) return;

	time_UTC_to_Beijing(utc_date, &beijing_date);

	printf("board_set_date_time: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
			beijing_date.year, beijing_date.month, beijing_date.day, beijing_date.hour, beijing_date.min, beijing_date.sec);
	DS1302_SetTime(&beijing_date);

	memset(&beijing_date, 0, sizeof(beijing_date));
	DS1302_GetTime(&beijing_date);
	printf("READBACK:  board_set_date_time: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
			beijing_date.year, beijing_date.month, beijing_date.day, beijing_date.hour, beijing_date.min, beijing_date.sec);
}

void board_set_unix_tick(uint32_t unix_tick)
{
	printf("board_set_unix_time: %d\r\n", unix_tick);
	DS1302_Write_Unix_Tick(unix_tick);
	printf("READBACK:  board_set_unix_time: %d\r\n", DS1302_Read_Unix_Tick());

	unix_tick_set = 1;
}

void board_init(void)
{
	printf("Ranqi board boot up......\r\n");
	Ranqi_Rx_Queue = xQueueCreateStatic( RANQI_QUEUE_DEPTH,
			                             sizeof(Ranqi_xMessage_s),
										 ucQueueStorageArea,
										 &xStaticQueue );
	if(!Ranqi_Rx_Queue) {
		printf("Create Ranqi_Rx_Queue...  Exit\r\n");
	}
	HAL_TIM_Base_Start_IT(&htim3);


    DS1302_Init();
    W25qxx_Init();
    L3G4200D_Init();  // g-sensor
    ad147a_init();    // 中压传感器
    atgm332d_init();  // GPS module

    mem_info();
}

void board_led_on(board_led_e led)
{
	if (led == BOARD_LED_SYS) {
		HAL_GPIO_WritePin(SYSTEM_LED_GPIO_Port, SYSTEM_LED_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin, GPIO_PIN_SET);
	}
}

void board_led_off(board_led_e led)
{
	if (led == BOARD_LED_SYS) {
		HAL_GPIO_WritePin(SYSTEM_LED_GPIO_Port, SYSTEM_LED_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin, GPIO_PIN_RESET);
	}
}

void board_led_toggle(board_led_e led)
{
	if (led == BOARD_LED_SYS) {
		HAL_GPIO_TogglePin(SYSTEM_LED_GPIO_Port, SYSTEM_LED_Pin);
	} else {
		HAL_GPIO_TogglePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin);
	}
}

uint32_t board_1s_tick_get(void)
{
	return board_1s_tick;
}

uint32_t board_1m_tick_get(void)
{
	return board_1m_tick;
}


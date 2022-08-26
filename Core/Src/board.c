
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rcc.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "Queue.h"

#include "gpio.h"
#include "board.h"


uint32_t board_1s_tick = 0;
uint32_t board_1m_tick = 0;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef  htim3;


extern QueueHandle_t Ranqi_Rx_Queue;
static StaticQueue_t xStaticQueue;
uint8_t ucQueueStorageArea[ RANQI_QUEUE_DEPTH * sizeof(Ranqi_xMessage_s) ];

extern void atClient_RxCpltCallback(UART_HandleTypeDef *huart);

void mem_info(void)
{
	printf("\n\rTotal_mem:%d freeMem:%d\r\n", configTOTAL_HEAP_SIZE, xPortGetFreeHeapSize());
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1) {
		atClient_RxCpltCallback(huart);
	} else if (huart == &huart2) {
//		gps_RxCpltCallback(huart);
	} else {

	}
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

    mem_info();
    DS1302_Init();
//    W25qxx_Init();    // SPI flash
//    L3G4200D_Init();  // g-sensor
//    atgm332d_init();  // GPS module
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


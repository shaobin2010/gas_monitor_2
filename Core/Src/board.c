
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rcc.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "Queue.h"

#include "board.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

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
    // HAL_TIM_Base_Start_IT(&htim3);
	printf("Ranqi board boot up......\r\n");
	Ranqi_Rx_Queue = xQueueCreateStatic( RANQI_QUEUE_DEPTH,
			                             sizeof(Ranqi_xMessage_s),
										 ucQueueStorageArea,
										 &xStaticQueue );
	if(!Ranqi_Rx_Queue) {
		printf("Create Ranqi_Rx_Queue...  Exit\r\n");
	}

    mem_info();
    DS1302_Init();
//    W25qxx_Init();    // SPI flash
//    L3G4200D_Init();  // g-sensor
//    atgm332d_init();  // GPS module
}

#include <include/at_log.h>
#include <include/config.h>
#include <include/hal_export.h>
#include <include/ring_buffer.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"
#include "main.h"

#include "include/at_export_error.h"

extern UART_HandleTypeDef huart1;
static UART_HandleTypeDef *pAtUart = &huart1;

/*uart data recv buff which used by at_client*/
static  uint8_t       at_byte;
extern  ring_buffer_t at_uart_rb;

void atClient_Start_Rx(void)
{
	HAL_UART_Receive_IT(&huart1, &at_byte, 1);
}

void atClient_RxCpltCallback(UART_HandleTypeDef *huart)
{
    ring_buffer_write(&at_uart_rb, &at_byte, 1);
    HAL_UART_Receive_IT(pAtUart, &at_byte, 1);
}

int atClient_Send_Data(char *pdata, uint32_t len)
{
	if(HAL_OK == HAL_UART_Transmit(pAtUart, pdata, len, 0xFFFF)){
		return len;
	} else {
		return 0;
	}
}

int module_power_on(void)
{
	Log_d("Power on the module");
	
	HAL_GPIO_WritePin(NB_nRST_GPIO_Port, NB_nRST_Pin, GPIO_PIN_RESET);
	osDelay(500);

	ring_buff_flush(&at_uart_rb);
	atClient_Start_Rx();

	HAL_GPIO_WritePin(NB_nRST_GPIO_Port, NB_nRST_Pin, GPIO_PIN_SET);
	osDelay(1000);

	return AT_SYS_RET_SUCCESS;
}


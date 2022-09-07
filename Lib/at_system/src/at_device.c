
#include <include/at_device.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"

#include "include/at_export_error.h"
#include "include/at_client.h"
#include "include/at_utils.h"
#include "include/at_log.h"
#include "include/at_device.h"

static at_device_s atDevice_dev;

#define Ctwing_Send_Max_Len    500  // 500*2 = 1000
uint8_t ctwing_send_buff[1024];

SemaphoreHandle_t ctwing_Lock_Mutex;

static Ranqi_xMessage_s ranqi_tx_msg;
extern QueueHandle_t Ranqi_Rx_Queue;

/*
  1.  优先初始化at_client_init， 启动AT模块处理线程
  2.  poweron 模块，并使能AT串口接收
*/
at_system_error_e at_device_init(void)
{
	memset(&atDevice_dev, 0, sizeof(atDevice_dev));
	ctwing_Lock_Mutex = xSemaphoreCreateMutex();
	if(ctwing_Lock_Mutex == NULL) {
		Log_e("at_device_init create mutex FAILED");
    	return AT_SYS_ERR_FAILURE;
	}

	while(at_client_status() < AT_STATUS_AT_READY) {
		osDelay(1000);
		continue;
	}

	if(module_get_imei(atDevice_dev.IMEI) != AT_SYS_RET_SUCCESS) {
		Log_e("Get IMEI FAILED");
		return AT_SYS_ERR_FAILURE;
	}
	if( module_get_iccid(atDevice_dev.ICCID) != AT_SYS_RET_SUCCESS) {
		Log_e("Get ICCID FAILED");
		return AT_SYS_ERR_FAILURE;
	}

	atDevice_dev.dev_state = At_Dev_State_Init;
    return AT_SYS_RET_SUCCESS;
}

at_system_error_e module_get_imei(char *imei)
{
	char *ptr;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);
	if(resp == NULL) {
		return AT_SYS_ERR_FAILURE;
	}

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+CGSN=1")) {
    	Log_e("AT+CGSN=1 exec err");
		result = AT_SYS_ERR_FAILURE;
	}

	// +CGSN: 86154 10696 52098
	ptr = strstr(resp->buf, "+CGSN:");
	if(ptr != NULL) {
		memcpy(imei, (ptr+6), 15);
		Log_i("module_get_imei: %s\r\n", (ptr+6));
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}

at_system_error_e module_get_rssi(uint8_t *rssi)
{
	char *ptr;
	uint8_t data;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+CSQ")) {
    	Log_e("AT+CSQ exec err");
		result = AT_SYS_ERR_FAILURE;
	}

	ptr = strstr(resp->buf, "+CSQ:");
	at_resp_parse_args(ptr, "+CSQ:%d,", &data);

	Log_i("rssi : %d\r\n", data);
	*rssi = data;

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}


at_system_error_e module_get_iccid(char *iccid)
{
	char *ptr;
	uint8_t data;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+NCCID")) {
    	Log_e("AT+NCCID exec err");
		result = AT_SYS_ERR_FAILURE;
	}

	// +NCCID:89860425102180462919
	ptr = strstr(resp->buf, "+NCCID:");
	if(ptr != NULL) {
		memcpy(iccid, (ptr+7), 20);
		Log_i("module_get_iccid: %s\r\n", (ptr+7));
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}


// 检查NB网附着状态
// 默认状态下， 第二位数据返回1或5 指示网络附着成功

at_system_error_e module_get_cereg(uint8_t *attach)
{
	char *ptr;
	uint8_t dummy, data;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+CEREG?")) {
    	Log_e("AT+CEREG? exec err");
		result = AT_SYS_ERR_FAILURE;
	}

	// +CEREG:1,1
	*attach = 0;
	ptr = strstr(resp->buf, "+CEREG:");
	at_resp_parse_args(ptr, "+CEREG:%d,%d", &dummy, &data);
	// printf("cereg : %d\r\n", data);
	if((data == 1) || (data == 5)) {
		*attach = 1;
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}


// 查询PDP激活状态
// 返回+CGATT:1， 表示PDP已激活
// 返回+CGATT:0， 表示PDP未激活
at_system_error_e module_get_cgatt(uint8_t *act)
{
	char *ptr;
	uint8_t data;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+CGATT?")) {
    	Log_e("AT+CGATT? exec err");
		result = AT_SYS_ERR_FAILURE;
	}

	// +CGATT:1
	// +CGATT:0
	ptr = strstr(resp->buf, "+CGATT:");
	at_resp_parse_args(ptr, "+CGATT:%d", &data);
	// printf("cgatt : %d\r\n", data);
	*act = data;

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}

char *atDevice_get_imei(void)
{
    return atDevice_dev.IMEI;
}

char *atDevice_get_iccid(void)
{
    return atDevice_dev.ICCID;
}

void atDevice_wait_network(void)
{
	uint8_t rssi, attach, act;
	while(1) {
	    module_get_rssi(&rssi);
	    module_get_cereg(&attach);
	    module_get_cgatt(&act);

	    if ((rssi != 99) && attach && act){
	    	atDevice_dev.dev_state = At_Dev_State_Net_Ready;
	    	break;
	    }
	    osDelay(1000);
	}
}

/***************************************************************************************/
/*                        CTwing Network Related                                       */
/***************************************************************************************/

void ctwing_event_handle(const char *buff, uint32_t size)
{
    char *ptr;
	int8_t data;
    (void) size;

    Log_i("ctwing_event_handle:   %s", buff);
    ptr = strstr(buff, "+MCWEVENT:");
    if(strstr(buff, ",")) {
        // 9, <mID>
        // 10, <mID>
        at_resp_parse_args(ptr, "+MCWEVENT:%d,", &data);
    } else {
        at_resp_parse_args(ptr, "+MCWEVENT:%d", &data);
    }

    switch (data) {
        case 1: // 注册成功
            Ctwing_Reg_Bit_Set(atDevice_dev.ctwing_flag);
            break;

        case 2: // 注册失败
        case 5:
            Ctwing_Reg_Bit_Clr(atDevice_dev.ctwing_flag);
            break;
        case 7:
            Ctwing_Sub_Bit_Set(atDevice_dev.ctwing_flag);
            break;
        case 8:
            Ctwing_Sub_Bit_Clr(atDevice_dev.ctwing_flag);
            break;

        default:
            printf("MCW event id is %d\r\n", data);
            break;
    }
}


// AT+MCWCREATE=221.229.214.202,5683
at_system_error_e ctwing_setup_server(uint32_t ip, uint16_t port)
{
	char *ptr;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;
	
	memset(ctwing_send_buff, 0, sizeof(ctwing_send_buff));
	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);

	sprintf(ctwing_send_buff, "AT+MCWCREATE=%d.%d.%d.%d,%d", 
	                            (int)(0xFF & (ip >> 24)),
								(int)(0xFF & (ip >> 16)),
								(int)(0xFF & (ip >>  8)),
								(int)(0xFF & ip),
								(int)port);

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, ctwing_send_buff)) {
    	Log_e("%s exec err", ctwing_send_buff);
		result = AT_SYS_ERR_FAILURE;
	}

	ptr = strstr(resp->buf, "OK");
	if(ptr == NULL) {
		result = AT_SYS_ERR_FAILURE;
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}

// AT+MCWOPEN=1,86400 自动更新注册
at_system_error_e ctwing_reg_server(void)
{
	char *ptr;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);
	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+MCWOPEN=1,86400")) {
    	Log_e("AT+MCWOPEN=1,86400 exec ERROR");
		result = AT_SYS_ERR_FAILURE;
	}

	ptr = strstr(resp->buf, "OK");
	if(ptr == NULL) {
		result = AT_SYS_ERR_FAILURE;
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}

at_system_error_e ctwing_dereg_server(void)
{
	char *ptr;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);
	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, "AT+MCWCLOSE")) {
    	Log_e("AT+MCWCLOSE exec ERROR");
		result = AT_SYS_ERR_FAILURE;
	}

	ptr = strstr(resp->buf, "OK");
	if(ptr == NULL) {
		result = AT_SYS_ERR_FAILURE;
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}


void atDevice_ctwing_connect(uint32_t server_ip, uint16_t server_port)
{
	uint8_t wait_time; 
    static uint8_t setup_server = 0;

    if(Ctwing_Ready(atDevice_dev.ctwing_flag)){
    	return;
    }

    if (!setup_server) {
    	ctwing_setup_server(server_ip, server_port);
    	setup_server = 1;
    }

retry:
	wait_time = 5; 
    ctwing_reg_server();
    while(!Ctwing_Ready(atDevice_dev.ctwing_flag)){
		if(!wait_time) {
			ctwing_dereg_server();
			goto retry;
		}
		wait_time--;
        osDelay(1000);
    }
}


static void hex_str_copy(uint8_t *dst, uint8_t *src, uint32_t len)
{
	uint32_t i;
	char high_part, low_part;
	for (i = 0; i < len; i++)
	{
		high_part = 0x0F & (src[i] >> 4);
		low_part = 0x0F & src[i];
		high_part = (high_part > 9) ? (high_part - 10 + 'A') : (high_part + '0');
		low_part = (low_part > 9) ? (low_part - 10 + 'A') : (low_part + '0');

		dst[2*i]   = high_part;
		dst[2*i+1] = low_part;
	}
}

static void str_hex_copy(uint8_t *dst, char *src, uint32_t len)
{
	uint32_t i;
	char high_part, low_part;
	for (i = 0; i < len; i++)
	{
		high_part = src[2*i];
		low_part  = src[2*i+1];
		if(high_part <= '9') {
			high_part = high_part - '0';
		} else if (high_part <= 'F') {
			high_part = high_part - 'A' + 10;
		} else {
			high_part = high_part - 'a' + 10;
		}

		if(low_part <= '9') {
			low_part = low_part - '0';
		} else if (low_part <= 'F') {
			low_part = low_part - 'A' + 10;
		} else {
			low_part = low_part - 'a' + 10;
		}

		dst[i] = (uint8_t)((high_part << 4) | low_part);
	}
}


// AT+MCWSEND=4,11111111,1
// AT+MCWSEND=4,11111111,0
at_system_error_e ctwing_send_data(uint8_t ack, uint8_t *buff, uint32_t len)
{
	char *ptr;
	uint32_t size;
	at_system_error_e result = AT_SYS_RET_SUCCESS;
	at_response_t resp = NULL;
	if(len > Ctwing_Send_Max_Len) {
		return AT_SYS_ERR_FAILURE;
	}

	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);
	size = sprintf(ctwing_send_buff, "AT+MCWSEND=%d,", len);
	hex_str_copy(&ctwing_send_buff[size], buff, len);
	size += len*2;
	size = sprintf(&ctwing_send_buff[size], ",%d", ack);

	if(AT_SYS_RET_SUCCESS != at_exec_cmd(resp, ctwing_send_buff)) {
    	Log_e("AT+MCWSEND=%d, exec err", len);
		result = AT_SYS_ERR_FAILURE;
	}

	ptr = strstr(resp->buf, "OK");
	if(ptr == NULL) {
		result = AT_SYS_ERR_FAILURE;
	}

	if(resp) {
		at_delete_resp(resp);
	}
	
    return result;
}


void ctwing_read_event(const char *buff, uint32_t size)
{
    char *ptr;
	uint32_t data_len;
    (void) size;

    ptr = strstr(buff, "+MCWREAD:");
	at_resp_parse_args(ptr, "+MCWREAD:%d,", &data_len);
	Log_i("ctwing_read_event receives %d bytes\r\n", data_len);
	if(data_len > RQNQI_QUEUE_ITEM_SIZE) {
		Log_e("ctwing_read_event [%d > %d]\r\n", data_len, RQNQI_QUEUE_ITEM_SIZE);
		return;
	}

	// Set data len
	ranqi_tx_msg.ucData_len = data_len;

	ptr = strstr(buff, ",");
	str_hex_copy(ranqi_tx_msg.ucData, ptr+1, data_len);

	if(xQueueSend(Ranqi_Rx_Queue,
	               ( void * ) &ranqi_tx_msg,
	               ( TickType_t ) 0 ) == errQUEUE_FULL) {
		Log_e("Ranqi_Rx_Queue is FULL\r\n");
	}
}




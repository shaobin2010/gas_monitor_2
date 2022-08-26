


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "include/ring_buffer.h"
#include "include/config.h"

#include "include/at_client.h"
#include "include/hal_export.h"

#include "include/at_export_error.h"
#include "include/at_sanity_check.h"
#include "include/at_utils.h"

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

static at_client sg_at_client;
SemaphoreHandle_t atClient_Lock_Mutex;
SemaphoreHandle_t atClient_Resp_Sema;

ring_buffer_t at_uart_rb;
char at_uart_rb_buff[RING_BUFF_LEN];
char at_recv_buff[CLINET_BUFF_LEN];

static void dummy_indi_handle(const char *data, uint32_t size);
static void ctzeu_indi_handle(const char *data, uint32_t size);
extern void ctwing_event_handle(const char *buff, uint32_t size);
extern void ctwing_read_event(const char *buff, uint32_t size);

static at_urc urc_table[] = {
        {"+CTZEU:",        "\r\n", ctzeu_indi_handle},
		{"+MODULE_READY:", "\r\n", ctzeu_indi_handle},
		{"^SIMST:",        "\r\n", ctzeu_indi_handle},
		{"+CGEV:",         "\r\n", dummy_indi_handle},

		{"+MCWEVENT",      "\r\n", ctwing_event_handle},
		{"+MCWREAD",       "\r\n", ctwing_read_event},
};


static void dummy_indi_handle(const char *data, uint32_t size)
{
	(void)data;
	(void)size;
}

static void ctzeu_indi_handle(const char *data, uint32_t size)
{
	sg_at_client.status = AT_STATUS_AT_READY;
	Log_d("sg_at_client.status = AT_STATUS_AT_READY");
}


static const at_urc *get_urc_obj(at_client_t client)
{
	int i, prefix_len, suffix_len;
	int buf_sz;
	char *buffer = NULL;

	if (client->urc_table == NULL) {
		return NULL;
	}

	buffer = client->recv_buffer;
	buf_sz = client->cur_recv_len;

	for (i = 0; i < client->urc_table_size; i++) {
		prefix_len = strlen(client->urc_table[i].cmd_prefix);
		suffix_len = strlen(client->urc_table[i].cmd_suffix);
		if (buf_sz < prefix_len + suffix_len) {
			continue;
		}
		if ((prefix_len ? !strncmp(buffer, client->urc_table[i].cmd_prefix, prefix_len) : 1)
				&& (suffix_len ? !strncmp(buffer + buf_sz - suffix_len, client->urc_table[i].cmd_suffix, suffix_len) : 1))
		{
			//Log_d("matched:%s", client->urc_table[i].cmd_prefix);
			return &client->urc_table[i];
		}
	}

	return NULL;
}


static at_system_error_e at_client_getchar(at_client_t client, char *pch, uint32_t timeout)
{
	int32_t ret;
	ret = ring_buffer_read(client->pRingBuff, (uint8_t *)pch, 1, timeout);

	if (ret > 0) {
		return AT_SYS_RET_SUCCESS;
	} else if (ret == -1) {
		return AT_SYS_ERR_INVAL;
	} else if(ret == -2) {
		return AT_SYS_ERR_TIMEOUT;
	} else {
		return AT_SYS_ERR_FAILURE;
	}
}

static int at_recv_readline(at_client_t client)
{
	int read_len = 0;
	char ch = 0, last_ch = 0;
	at_system_error_e ret;

	memset(client->recv_buffer, 0x00, client->recv_bufsz);
	client->cur_recv_len = 0;

	while (1) {
		ret = at_client_getchar(client, &ch, GET_CHAR_TIMEOUT_MS);  // 5秒钟检测一个字符
		if(AT_SYS_RET_SUCCESS != ret) {
			printf("at_client_getchar   ret %d\r\n", ret);
			continue;
		}
		if (!isprint(ch) && (ch != '\r') && (ch != '\n')) {
			continue;
		}

		if (read_len < client->recv_bufsz) {
			client->recv_buffer[read_len++] = ch;
			client->cur_recv_len = read_len;
		} else {
			Log_e("read line failed. The line data length is out of buffer size(%d)!", client->recv_bufsz);
			memset(client->recv_buffer, 0x00, client->recv_bufsz);
			client->cur_recv_len = 0;
			read_len = 0;
			ring_buff_flush(client->pRingBuff);
			// return AT_SYS_ERR_FAILURE;
		}

		if((read_len == 2) && (client->recv_buffer[1] == '\n') && (client->recv_buffer[0] == '\r')) {
			memset(client->recv_buffer, 0x00, client->recv_bufsz);
			client->cur_recv_len = 0;
			read_len = 0;
			continue;
		}

		/* is newline or URC data */
		if ( (ch == '\n' && last_ch == '\r') ||
		    (client->end_sign != 0 && ch == client->end_sign) ||
			get_urc_obj(client)) {

			break;
		}

		last_ch = ch;
	}

#ifdef AT_PRINT_RAW_CMD
	at_print_raw_cmd("recvline", client->recv_buffer, read_len);
#endif

	return read_len;
}


static at_system_error_e at_client_para_init(at_client_t client)
{
	// reset at_client structure
	memset((void *)client, 0, sizeof(at_client));

	client->status = AT_STATUS_UNINITIALIZED;
	// client->end_sign;

	ring_buffer_init(&at_uart_rb, at_uart_rb_buff, RING_BUFF_LEN);
	client->pRingBuff = &at_uart_rb;

	client->recv_buffer = at_recv_buff;
	client->recv_bufsz = CLINET_BUFF_LEN;
	client->cur_recv_len = 0;
	client->resp = NULL;
	client->resp_status = AT_RESP_OK;

	client->urc_table = urc_table;
	client->urc_table_size = sizeof(urc_table) / sizeof(urc_table[0]);
	return AT_SYS_RET_SUCCESS;
}


static at_system_error_e at_client_init(at_client_t pClient)
{
	at_system_error_e result;
	POINTER_SANITY_CHECK(pClient, AT_SYS_ERR_NULL);


	atClient_Lock_Mutex = xSemaphoreCreateMutex();
	if(atClient_Lock_Mutex == NULL) {
		Log_e("at_client_init create mutex failed");
    	return AT_SYS_ERR_FAILURE;
	}

	atClient_Resp_Sema = xSemaphoreCreateBinary();
	if(atClient_Resp_Sema == NULL) {
		Log_e("at_client_init create sema failed");
    	return AT_SYS_ERR_FAILURE;
	}

	result = at_client_para_init(pClient);
	if (result == AT_SYS_RET_SUCCESS) {
		Log_d("at_client_init initialize success.");
		pClient->status = AT_STATUS_INITIALIZED;
	} else {
		Log_e("at_client_init initialize failed.");
	}

	return result;
}

void AT_Client_Task( void *pvParameters )
{
	int resp_buf_len = 0;
	const at_urc *urc;
	int line_counts = 0;

	at_client_t p_client = &sg_at_client;

	if(AT_SYS_RET_SUCCESS != at_client_init(p_client)) {
		Log_e("AT_Client_Task failed, Exit Task");
		return;
	}

	module_power_on();
	Log_d("AT_Client_Task starting...");
    while(1)
    {
    	if(at_recv_readline(p_client) > 0) {
#ifdef 	AT_PRINT_RAW_CMD
			const char *cmd = NULL;
			int cmdsize = 0;
			cmd = at_get_last_cmd(&cmdsize);
			Log_d("last_cmd:(%.*s), readline:%s",  cmdsize, cmd, p_client->recv_buffer);
#endif
			if ((urc = get_urc_obj(p_client)) != NULL) {
				/* current receive is request, try to execute related operations */
				if (urc->func != NULL) {
					urc->func(p_client->recv_buffer, p_client->cur_recv_len);  // 处理响应
				}
			} else if (p_client->resp != NULL) {
				/* current receive is response */
				p_client->recv_buffer[p_client->cur_recv_len - 1] = '\0';
				if (resp_buf_len + p_client->cur_recv_len < p_client->resp->buf_size) {
					/* copy response lines, separated by '\0' */
					memcpy(p_client->resp->buf + resp_buf_len, p_client->recv_buffer, p_client->cur_recv_len);
					resp_buf_len += p_client->cur_recv_len;

					line_counts++;
				} else {
					p_client->resp_status = AT_RESP_BUFF_FULL;
					Log_e("Read response buffer failed. The Response buffer size is out of buffer size(%d)!", p_client->resp->buf_size);
				}

				/* check response result */
				if (memcmp(p_client->recv_buffer, AT_RESP_END_OK, strlen(AT_RESP_END_OK)) == 0
							&& p_client->resp->line_num == 0) {
					/* get the end data by response result, return response state END_OK. */
					p_client->resp_status = AT_RESP_OK;
				} else if (strstr(p_client->recv_buffer, AT_RESP_END_ERROR)
					      || (memcmp(p_client->recv_buffer, AT_RESP_END_FAIL, strlen(AT_RESP_END_FAIL)) == 0)) {
					p_client->resp_status = AT_RESP_ERROR;
				} else if (line_counts == p_client->resp->line_num && p_client->resp->line_num) {
					/* get the end data by response line, return response state END_OK.*/
					p_client->resp_status = AT_RESP_OK;
				} else {
					continue;
				}
				p_client->resp->line_counts = line_counts;

				p_client->resp = NULL;
				resp_buf_len = 0;
				line_counts = 0;
				xSemaphoreGive( atClient_Resp_Sema );
			} else {
				Log_d("unrecognized line: %.*s", p_client->cur_recv_len, p_client->recv_buffer);
			}
    	}
    }
}

at_system_error_e at_obj_exec_cmd(at_response_t resp, const char *cmd_expr, ...)
{
	POINTER_SANITY_CHECK(cmd_expr, AT_SYS_ERR_NULL);

	va_list args;
	int cmd_size = 0;

	at_system_error_e result = AT_SYS_RET_SUCCESS;
	const char *cmd = NULL;
	at_client_t client =  &sg_at_client;

	if(xSemaphoreTake(atClient_Lock_Mutex, portMAX_DELAY) == pdFALSE) {
		Log_e("at_obj_exec_cmd get atClient_Lock_Mutex FAILED!");
		return AT_SYS_ERR_FAILURE;
	}
	Log_i("at_obj_exec_cmd atClient_Lock_Mutex LOCKED");

//	resp->line_counts = 0;
	client->resp_status = AT_RESP_OK;
	client->resp = resp;

	va_start(args, cmd_expr);
	at_vprintfln(cmd_expr, args);
	va_end(args);

	if (resp != NULL) {
		if(xSemaphoreTake(atClient_Resp_Sema, resp->timeout*portTICK_PERIOD_MS ) == pdFALSE) {  // Timeout
			cmd = at_get_last_cmd(&cmd_size);
			Log_e("execute command (%.*s) timeout %dms!", cmd_size, cmd, resp->timeout);
			client->resp_status = AT_RESP_TIMEOUT;
			result = AT_SYS_ERR_NULL;
		} else {
			if (client->resp_status != AT_RESP_OK) {
				cmd = at_get_last_cmd(&cmd_size);
				Log_e("execute command (%.*s) FAILED", cmd_size, cmd);
				result = AT_SYS_ERR_FAILURE;
			}
		}
	}

	 client->resp = NULL;
	if (xSemaphoreGive(atClient_Lock_Mutex) != pdTRUE) {
		Log_e("at_obj_exec_cmd release atClient_Lock_Mutex FAILED!");
		return AT_SYS_ERR_FAILURE;
	}
	Log_i("at_obj_exec_cmd atClient_Lock_Mutex UNLOCKED");
	return result;
}

int at_client_status(void)
{
	return sg_at_client.status;
}

at_response_t at_create_resp(uint32_t buf_size, uint32_t line_num, uint32_t timeout)
{
	at_response_t resp = NULL;

	resp = (at_response_t)pvPortMalloc(sizeof(at_response));
	if (resp == NULL) {
		Log_e("AT create response object failed! No memory for response object!");
		return NULL;
	}

	resp->buf = (char *) pvPortMalloc(buf_size);
	if (resp->buf == NULL) {
		Log_e("AT create response object failed! No memory for response buffer!");
		vPortFree(resp);
		return NULL;
	}

	resp->buf_size = buf_size;
	resp->line_num = line_num;
	resp->line_counts = 0;
	resp->timeout = timeout;
	return resp;
}

void at_delete_resp(at_response_t resp)
{
    if (resp && resp->buf) {
        vPortFree(resp->buf);
    }

    if (resp) {
        vPortFree(resp);
        resp = NULL;
    }
}






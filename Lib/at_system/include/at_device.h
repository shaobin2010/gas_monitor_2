
#ifndef _AT_DEVICE_H_
#define _AT_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "semphr.h"

#include "include/hal_export.h"
#include "include/at_export_error.h"
#include "include/config.h"

#define IMEI_LEN            15   //
#define ICCID_LEN           20   // 89861118216007272115

#define Ctwing_Reg_Bit      (1 << 0)
#define Ctwing_Sub_Bit      (1 << 1)
#define Ctwing_Reg_Bit_Set(val)  val |= Ctwing_Reg_Bit
#define Ctwing_Sub_Bit_Set(val)  val |= Ctwing_Sub_Bit
#define Ctwing_Reg_Bit_Clr(val)  val &= ~Ctwing_Reg_Bit
#define Ctwing_Sub_Bit_Clr(val)  val &= ~Ctwing_Sub_Bit
#define Ctwing_Ready(val)        (val == (Ctwing_Reg_Bit | Ctwing_Sub_Bit))

typedef enum {
	At_Dev_State_Init       = 0,
	At_Dev_State_Net_Ready  = 1,
} at_device_state_e;

typedef struct {
	uint8_t dev_state;
    uint8_t  ctwing_flag;
    char IMEI[IMEI_LEN+1];
    char ICCID[ICCID_LEN+1];
} at_device_s;

extern SemaphoreHandle_t ctwing_Lock_Mutex;
at_system_error_e at_device_init(void);

extern at_system_error_e module_get_imei(char *imei);
extern at_system_error_e module_get_iccid(char *iccid);
extern at_system_error_e module_get_rssi(uint8_t *rssi);
extern at_system_error_e module_get_cereg(uint8_t *attach);
extern at_system_error_e module_get_cgatt(uint8_t *act);
extern char *atDevice_get_imei(void);
extern char *atDevice_get_iccid(void);
extern void atDevice_wait_network(void);
extern void atDevice_ctwing_connect(uint32_t server_ip, uint16_t server_port);


extern at_system_error_e ctwing_setup_server(uint32_t ip, uint16_t port);
extern at_system_error_e ctwing_reg_server(void);
extern at_system_error_e ctwing_send_data(uint8_t ack, uint8_t *buff, uint32_t len);
#define CTWING_SEND_DATA_L(ack, buff, len)                    ({   \
			                                                  	  at_system_error_e ret; \
			                                                  	  xSemaphoreTake(ctwing_Lock_Mutex, portMAX_DELAY); \
			                                                  	  ret = ctwing_send_data(ack, buff, len); \
			                                                  	  xSemaphoreGive(ctwing_Lock_Mutex); \
			                                                  	  ret; \
                                                              })


#ifdef __cplusplus
}
#endif
#endif

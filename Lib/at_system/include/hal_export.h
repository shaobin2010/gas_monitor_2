
#ifndef __HAL_EXPORT_H__
#define __HAL_EXPORT_H__


#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/config.h"

#define 	HAL_AT_ERROR	   -1
#define 	HAL_AT_OK		   0

//#define 	AT_PRINT_RAW_CMD


//#ifdef OS_USED
//#include "cmsis_os.h"
//#endif




int atClient_Send_Data(char *pdata, uint32_t len);
int module_power_on(void);

#define HAL_Printf(format, ...)   do { \
                                         printf(format, ##__VA_ARGS__); \
                                     } while(0)


int HAL_Snprintf(char *str, const int len, const char *fmt, ...);
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap);

void HAL_DelayMs(uint32_t ms);
void HAL_DelayUs(uint32_t us);
uint32_t HAL_GetTimeMs(void);
uint32_t HAL_GetTimeSeconds(void);

int HAL_SetDevInfo(void *pdevInfo);
int HAL_GetDevInfo(void *pdevInfo);


#endif

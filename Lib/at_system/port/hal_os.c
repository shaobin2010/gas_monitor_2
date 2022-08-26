
#include <include/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"


int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_DelayMs(uint32_t ms)
{
   HAL_Delay(ms);
}

void HAL_DelayUs(uint32_t us)
{
	__IO uint32_t Delay = us * 72 / 8;
	do {
		__NOP();
	} while(Delay--);
}

uint32_t HAL_GetTimeMs(void)
{
    return HAL_GetTick();
}

uint32_t HAL_GetTimeSeconds(void)
{
    return HAL_GetTimeMs()/1000;
}


#ifdef OS_USED
void hal_thread_create(volatile void* threadId, uint16_t stackSize, int Priority, void (*fn)(void*), void* arg)
{
	osThreadDef(parseTask, (os_pthread)fn, (osPriority)Priority, 0, stackSize);
	threadId = osThreadCreate(osThread(parseTask), arg);
	(void)threadId; //Eliminate warning
}

void hal_thread_destroy(void* threadId)
{
	osThreadTerminate(threadId);
}

void HAL_SleepMs(uint32_t ms)
{
   (void)osDelay(ms);
}

void *HAL_Malloc(uint32_t size)
{
	return pvPortMalloc(size);
}

void HAL_Free(void *ptr)
{
    vPortFree(ptr);
}


#else
void hal_thread_create(void** threadId, void (*fn)(void*), void* arg)
{

}

void HAL_SleepMs(_IN_ uint32_t ms)
{
	(void)HAL_Delay(ms);
}

void *HAL_Malloc(_IN_ uint32_t size)
{
	return malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
   free(ptr);
}

void *HAL_MutexCreate(void)
{
	return (void *)1;
}


void HAL_MutexDestroy(_IN_ void* mutex)
{
	return;
}

void HAL_MutexLock(_IN_ void* mutex)
{
	return;
}

void HAL_MutexUnlock(_IN_ void* mutex)
{
	return;
}

#endif

int HAL_SetDevInfo(void *pdevInfo)
{
	(void) pdevInfo;
#if 0
	int ret = QCLOUD_RET_SUCCESS;;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;


	if(NULL == pdevInfo){
		return QCLOUD_ERR_FAILURE;
	}

#ifdef DEBUG_DEV_INFO_USED
	memset(sg_product_id, '\0', MAX_SIZE_OF_PRODUCT_ID);
	memset(sg_device_name, '\0', MAX_SIZE_OF_DEVICE_NAME);

	strncpy(sg_product_id, devInfo->product_id, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(sg_device_name, devInfo->device_name, MAX_SIZE_OF_DEVICE_NAME);

#ifdef DEV_DYN_REG_ENABLED
	memset(sg_product_secret, '\0', MAX_SIZE_OF_PRODUCT_SECRET);
	strncpy(sg_product_secret, devInfo->product_secret, MAX_SIZE_OF_PRODUCT_KEY);
#endif

#ifdef 	AUTH_MODE_CERT
	memset(sg_device_cert_file_name, '\0', MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	memset(sg_device_privatekey_file_name, '\0', MAX_SIZE_OF_DEVICE_KEY_FILE_NAME);

	strncpy(sg_device_cert_file_name, devInfo->devCertFileName, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	strncpy(sg_device_privatekey_file_name, devInfo->devPrivateKeyFileName, MAX_SIZE_OF_DEVICE_KEY_FILE_NAME);
#else
	memset(sg_device_secret, '\0', MAX_SIZE_OF_DEVICE_SERC);
	strncpy(sg_device_secret, devInfo->devSerc, MAX_SIZE_OF_DEVICE_SERC);
#endif

#else
	 Log_e("HAL_SetDevInfo is not implement");
	 (void)devInfo; //eliminate compile warning

	 return QCLOUD_ERR_FAILURE;

#endif
#endif
	return 0;
}

int HAL_GetDevInfo(void *pdevInfo)
{
	(void)pdevInfo;
#if 0
	int ret = QCLOUD_RET_SUCCESS;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;

	if(NULL == pdevInfo){
		return QCLOUD_ERR_FAILURE;
	}

	memset((char *)devInfo, '\0', sizeof(DeviceInfo));

#ifdef DEBUG_DEV_INFO_USED

	strncpy(devInfo->product_id, sg_product_id, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(devInfo->device_name, sg_device_name, MAX_SIZE_OF_DEVICE_NAME);

#ifdef DEV_DYN_REG_ENABLED
	memset(devInfo->product_secret, '\0', MAX_SIZE_OF_PRODUCT_SECRET);
	strncpy(devInfo->product_secret, sg_product_secret, MAX_SIZE_OF_PRODUCT_SECRET);
#endif

#ifdef 	AUTH_MODE_CERT
	strncpy(devInfo->devCertFileName, sg_device_cert_file_name, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	strncpy(devInfo->devPrivateKeyFileName, sg_device_privatekey_file_name, MAX_SIZE_OF_DEVICE_KEY_FILE_NAME);
#else
	strncpy(devInfo->devSerc, sg_device_secret, MAX_SIZE_OF_DEVICE_SERC);
#endif

#else
   Log_e("HAL_GetDevInfo is not implement");

   return QCLOUD_ERR_FAILURE;
#endif

#endif
	return 0;
}



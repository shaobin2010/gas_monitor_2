
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "atgm332d.h"

static uint8_t one_byte;
static uint8_t USART_RX_BUF[USART_REC_LEN];
static uint16_t buf_pos;
static _SaveData Save_Data;

static void clrStruct(void)
{
	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;
	memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      // 清空
	memset(Save_Data.UTCTime, 0, UTCTime_Length);
	memset(Save_Data.latitude, 0, latitude_Length);
	memset(Save_Data.N_S, 0, N_S_Length);
	memset(Save_Data.longitude, 0, longitude_Length);
	memset(Save_Data.E_W, 0, E_W_Length);
}

void atgm332d_init(void)
{
    HAL_StatusTypeDef ret;
    memset(USART_RX_BUF, 0, USART_REC_LEN);
    clrStruct();
    ret = HAL_UART_Receive_IT(&huart2, &one_byte, 1);
    if(ret != HAL_OK) {
        printf("ATGM332D init failed\r\n");
    }
}

void gps_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(one_byte == '$') {
		buf_pos = 0;	
	}
	USART_RX_BUF[buf_pos++] = one_byte;

	if(USART_RX_BUF[0] == '$' && USART_RX_BUF[4] == 'M' && USART_RX_BUF[5] == 'C') { // 确定是否收到"GPRMC/GNRMC"这一帧数据
		if(one_byte == '\n') {
			memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      // 清空
			memcpy(Save_Data.GPS_Buffer, USART_RX_BUF, buf_pos); 	 // 保存数据
			Save_Data.isGetData = true;
			buf_pos = 0;
			memset(USART_RX_BUF, 0, USART_REC_LEN);                   // 清空				
		}
	}

	if(buf_pos >= USART_REC_LEN) {
		buf_pos = USART_REC_LEN - 1;
	}

    HAL_UART_Receive_IT(&huart2, &one_byte, 1);
}

// $GNRMC,051716.000,A,3107.34381,N,12135.26575,E,0.56,0.00,270822,,,E*71
void atgm332d_parse_gps_buffer(void)
{
	char *subString;
	char *subStringNext;
	char i = 0;

	if (Save_Data.isGetData) {
		Save_Data.isGetData = false;  
        Save_Data.isParseData = false;
        Save_Data.isUsefull = false;

//		 printf("**************\r\n");
//		 printf(Save_Data.GPS_Buffer);
//		 printf("\r\n");

		for (i = 0 ; i <= 9 ; i++) {
			if (i == 0) {
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					return;	// 解析错误
			} else {
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL) {
					char usefullBuffer[2]; 
					switch(i) {
						case 1:  	// 获取UTC时间
                            memcpy(Save_Data.UTCTime, subString, 6);
                            break;
						case 2:
                            memcpy(usefullBuffer, subString, 1);
                            break;	
						case 3:    //  获取纬度信息
                            memcpy(Save_Data.latitude, subString, subStringNext - subString);
                            break;	
						case 4:    //  获取N/S
                            memcpy(Save_Data.N_S, subString, subStringNext - subString);
                            break;	
						case 5:   //  获取经度信息
                            memcpy(Save_Data.longitude, subString, subStringNext - subString);
                            break;	
						case 6:  //  获取E/W
                            memcpy(Save_Data.E_W, subString, subStringNext - subString);
                            break;
						case 9:
							memcpy(Save_Data.UTCDate, subString, 6);
							break;

						case 7:
						case 8:
						default: 
                            break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;
				} else {
					return;	//解析错误
				}
			}
		}
	}
}

// Get UTC date time
bool atgm332d_get_date_time(ds_time_s *utc_date)
{
    if ( Save_Data.isParseData == true ) {

    	utc_date->year  = (uint8_t)((Save_Data.UTCDate[4] - '0')*10 + (Save_Data.UTCDate[5] - '0'));
    	utc_date->month = (uint8_t)((Save_Data.UTCDate[2] - '0')*10 + (Save_Data.UTCDate[3] - '0'));
    	utc_date->day   = (uint8_t)((Save_Data.UTCDate[0] - '0')*10 + (Save_Data.UTCDate[1] - '0'));
    	utc_date->hour  = (uint8_t)((Save_Data.UTCTime[0] - '0')*10 + (Save_Data.UTCTime[1] - '0'));
    	utc_date->min   = (uint8_t)((Save_Data.UTCTime[2] - '0')*10 + (Save_Data.UTCTime[3] - '0'));
    	utc_date->sec   = (uint8_t)((Save_Data.UTCTime[4] - '0')*10 + (Save_Data.UTCTime[5] - '0'));
        return true;
    }
    return false;
}

// Save_Data.latitude = 3107.29526
// Save_Data.longitude = 12135.27842

bool atgm332d_get_latitude(float *lat)
{
    if ( (Save_Data.isParseData == true) && 
	     (Save_Data.isUsefull == true) ) {
			
        *lat  = (float)atof(Save_Data.latitude);
        return true;
    }
    return false;
}

bool atgm332d_get_longitude(float *lon)
{
    if ( (Save_Data.isParseData == true) && 
	     (Save_Data.isUsefull == true) ) {
        *lon  = (float)atof(Save_Data.longitude);
        return true;
    }
    return false;
}


bool atgm332d_get_n_s(char *n_s)
{
    if ( (Save_Data.isParseData == true) && 
	     (Save_Data.isUsefull == true) ) {
		if(strstr(Save_Data.N_S, 'N')) {
			*n_s = 'N';
		} else {
			*n_s = 'S';
		}
        return true;
    }
    return false;
}

bool atgm332d_get_e_w(char *e_w)
{
    if ( (Save_Data.isParseData == true) && 
	     (Save_Data.isUsefull == true) ) {
		if(strstr(Save_Data.E_W, 'E')) {
			*e_w = 'E';
		} else {
			*e_w = 'W';
		}
        return true;
    }
    return false;
}

void atgm332d_print_gps_info(void)
{
	if (Save_Data.isParseData)
	{	
		printf("Save_Data.UTCTime = ");
		printf(Save_Data.UTCTime);
		printf("\r\n");
		printf("Save_Data.UTCDate = ");
		printf(Save_Data.UTCDate);
		printf("\r\n");

		if(Save_Data.isUsefull)
		{
			printf("Save_Data.latitude = ");
			printf(Save_Data.latitude);
			printf("\r\n");


			printf("Save_Data.N_S = ");
			printf(Save_Data.N_S);
			printf("\r\n");

			printf("Save_Data.longitude = ");
			printf(Save_Data.longitude);
			printf("\r\n");

			printf("Save_Data.E_W = ");
			printf(Save_Data.E_W);
			printf("\r\n");
		}
		else
		{
			printf("GPS DATA is not usefull!\r\n");
		}
		
	}
}

#ifndef __ATGM332D_H__
#define __ATGM332D_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "main.h"
#include "usart.h"

#define USART_REC_LEN  			200 // 定义最大接收字节数 200

// 定义数组长度
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		// 是否获取到GPS数据
	char isParseData;	// 是否解析完成
	char UTCTime[UTCTime_Length];	    // UTC时间
	char UTCDate[UTCTime_Length];	    // UTC日期
	char latitude[latitude_Length];		// 纬度
	char N_S[N_S_Length];		        // N/S
	char longitude[longitude_Length];		// 经度
	char E_W[E_W_Length];		// E/W
	char isUsefull;		// 定位信息是否有效
} _SaveData;

extern void atgm332d_init(void);
extern void atgm332d_print_gps_info(void);
extern void atgm332d_parse_gps_buffer(void);
extern bool atgm332d_get_date_time(ds_time_s *utc_date);

extern bool atgm332d_get_latitude(float *lat);
extern bool atgm332d_get_longitude(float *lon);
extern bool atgm332d_get_n_s(char *n_s);
extern bool atgm332d_get_e_w(char *e_w);
/*

$GNRMC, 定位信息数据格式
034048.000,  // UTC时间：时分秒
A,           // 定位状态， A=有效定位， V=无效定位
3107.29526,  // 纬度ddmm.mmmm(度分)
N,           // 纬度半球： N北半球，S南半球
12135.27842, // 经度ddmm.mmmm(度分)
E,           // 经度半球： E东经，W西经
8.87,        // 地面速度
162.60,      // 地面航向
130822,      // UTC日期：ddmmyy日月年
,            // 磁偏角
,            // 磁偏角方向，E（东）或W（西）
A*75         // 模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）


Save_Data.UTCTime = 034048.000
Save_Data.latitude = 3107.29526
Save_Data.N_S = N
Save_Data.longitude = 12135.27842
Save_Data.E_W = E
*/

#ifdef __cplusplus
}
#endif
#endif /*__ATGM332D_H__ */


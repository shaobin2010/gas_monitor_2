#ifndef __DS1302_H__
#define __DS1302_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

extern void DS1302_Init(void);
#if 0
extern void DS1302_GetYear(uint8_t *year);
extern void DS1302_GetMonth(uint8_t *month);
extern void DS1302_GetDate(uint8_t *date);
extern void DS1302_GetHour(uint8_t *hour);
extern void DS1302_GetMinite(uint8_t *minute);
extern void DS1302_GetSecond(uint8_t *second);
#endif
extern void DS1302_GetTime(ds_time_s *time);
extern void DS1302_SetTime(ds_time_s *time);
extern void time_UTC_to_Beijing(ds_time_s *utc_time, ds_time_s *beijing_time);
extern void time_Beijing_to_UTC(ds_time_s *beijing_time, ds_time_s *utc_time);
extern uint32_t DS1302_Read_Unix_Tick(void);
extern void DS1302_Write_Unix_Tick(uint32_t unix_time);

#ifdef __cplusplus
}
#endif
#endif /*__ __DS1302_H__ */

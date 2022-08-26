#ifndef __DS1302_H__
#define __DS1302_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} ds_time_s;

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
extern void DS1302_SetTime(uint8_t yr, uint8_t mon, uint8_t date, uint8_t hr, uint8_t min, uint8_t sec);
extern void unix_to_UTC(uint32_t unix_time, ds_time_s *utc_time);
extern void UTC_to_unix(ds_time_s utc_time, uint32_t *unix_time);
extern uint32_t DS1302_Read_Unix_Time(void);
extern void DS1302_Write_Unix_Time(uint32_t unix_time);

#ifdef __cplusplus
}
#endif
#endif /*__ __DS1302_H__ */

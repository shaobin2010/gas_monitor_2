#ifndef __DS1302_H__
#define __DS1302_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

#define DS1302_Init()

extern void DS1302_GetTime(ds_time_s *time);
extern void DS1302_SetTime(ds_time_s *time);
extern void time_UTC_to_Beijing(ds_time_s *utc_time, ds_time_s *beijing_time);
extern void time_Beijing_to_UTC(ds_time_s *beijing_time, ds_time_s *utc_time);
extern uint32_t DS1302_Read_Unix_Tick(void);
extern void DS1302_Write_Unix_Tick(uint32_t unix_time);

#ifdef __cplusplus
}
#endif
#endif /*__DS1302_H__ */

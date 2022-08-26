#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ranqi_logical.h"

// 采样时间间隔
// 上报时间间隔
/*
说明：x---采集值
      a---超压上限
      b---超压下限
      c---欠压上限
      d---欠压下限
      当x≥a    超压上限报警
      当a>x≥b  超压下限报警
      当b>x>c   正常
      当c≥x>d  欠压上限报警
      当d≥x    欠压下限报警
*/

uint32_t ranqi_tick_1s = 0;
uint32_t ranqi_tick_1m = 0;


static bool is_1m_tick_timeout(uint8_t min)
{
      bool ret = false;
      static uint32_t last_tick_1m = 0xFFFFFFFF;
      if ((last_tick_1m !=ranqi_tick_1m) && (ranqi_tick_1m % min == 0)) {
            ret = true;
      }
      last_tick_1m = ranqi_tick_1m;
      return ret;
}


extern void atgm332d_print_gps_info(void);

void ranqi_tick_test(void)
{
      if(ranqi_tick_1s % 5 == 0){
            printf("111111111111111\r\n");
      }
      
      if(is_1m_tick_timeout(2)) {
            atgm332d_print_gps_info();
      }
}

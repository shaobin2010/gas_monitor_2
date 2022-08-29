
#include "main.h"
#include "gpio.h"
#include "ds1302.h"

#define SEC             0x40
#define MIN             0x41
#define HR              0x42
#define DATE            0x43
#define MONTH           0x44
#define DAY             0x45
#define YEAR            0x46
#define CONTROL         0x47
#define TRACKLE_CHARGER 0x48
#define CLOCK_BURST     0x5F
#define RAM0            0x60
#define RAM1            0x61
#define RAM30           0x7E
#define RAM_BURST       0x7F


// CLK -> PB13
// DAT -> PB14
// OE  -> PB15
#define OE_HIGH      HAL_GPIO_WritePin(DS1302_OE_GPIO_Port, DS1302_OE_Pin, GPIO_PIN_SET);
#define OE_LOW        HAL_GPIO_WritePin(DS1302_OE_GPIO_Port, DS1302_OE_Pin, GPIO_PIN_RESET)
#define CLK_HIGH      HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_SET)
#define CLK_LOW       HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET)
#define DAT_HIGH      HAL_GPIO_WritePin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin, GPIO_PIN_SET)
#define DAT_LOW       HAL_GPIO_WritePin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin, GPIO_PIN_RESET)

#define DAT           HAL_GPIO_ReadPin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin)


static
void ds_delay(int i)
{
	while (i) {
		i--;
	}
}

static void DS1302_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DS1302_DAT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &GPIO_InitStruct);
}

static void DS1302_In(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DS1302_DAT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &GPIO_InitStruct);
}

static void DS1302_WriteByte(uint8_t value)
{
	uint8_t i;
	DS1302_Out();
	for (i=0; i<8; i++){
		CLK_LOW;
		if (value&0x01) {
			DAT_HIGH;
		} else {
			DAT_LOW;
		}
		ds_delay(1);
		CLK_HIGH;
		ds_delay(1);		
		value >>= 1;
	}
}

static void DS1302_ReadByte(uint8_t *value) 
{
	uint8_t i;
	DS1302_In();
	for (i=0;i<8;i++) {
		*value >>= 1;
		CLK_HIGH;
		ds_delay(1);
		CLK_LOW;
		ds_delay(1);
		if (DAT == 1) {
			*value |= 0x80;
		} else {
			*value &= 0x7F;
		}
	}
}

static void DS1302_ReadReg(uint8_t addr, uint8_t *value) 
{
	CLK_LOW;
	OE_HIGH;
	DS1302_WriteByte((addr<<1)|0x01);
	DS1302_ReadByte(value);
	OE_LOW;
}

static void DS1302_WriteReg(uint8_t addr, uint8_t value) 
{
	CLK_LOW;
	OE_HIGH;
	DS1302_WriteByte((addr<<1)&0xfe);
	DS1302_WriteByte(value);
	CLK_LOW;
	OE_LOW;
}

#if 0
void DS1302_GetYear(uint8_t *year) 
{
	uint8_t value;
	DS1302_ReadReg(YEAR, &value);
	*year = ((value&0xf0)>>4)*10 + (value&0x0f);
}

void DS1302_GetMonth(uint8_t *month) 
{
	uint8_t value;
	DS1302_ReadReg(MONTH, &value);
	*month = ((value&0x10)>>4)*10 + (value&0x0f);
}

void DS1302_GetDate(uint8_t *date) 
{
	uint8_t value;
	DS1302_ReadReg(DATE, &value);
	*date = ((value&0x30)>>4)*10 + (value&0x0f);
}

void DS1302_GetHour(uint8_t *hour) 
{
	uint8_t value;
	DS1302_ReadReg(HR, &value);
	*hour = ((value&0x10)>>4)*10 + (value&0x0f);
}

void DS1302_GetMinite(uint8_t *minute) 
{
	uint8_t value;
	DS1302_ReadReg(MIN, &value);
	*minute = ((value&0x70)>>4)*10 + (value&0x0f);
}

void DS1302_GetSecond(uint8_t *second) 
{
	uint8_t value;
	DS1302_ReadReg(SEC, &value);
	*second = ((value&0x70)>>4)*10 + (value&0x0f);
}
#endif

void DS1302_GetTime(ds_time_s *time)
{
	uint8_t value;
	DS1302_ReadReg(YEAR, &value);
	time->year = ((value&0xf0)>>4)*10 + (value&0x0f);

	DS1302_ReadReg(MONTH, &value);
	time->month = ((value&0x10)>>4)*10 + (value&0x0f);

	DS1302_ReadReg(DATE, &value);
	time->day = ((value&0x30)>>4)*10 + (value&0x0f);

	DS1302_ReadReg(HR, &value);
	time->hour = ((value&0x10)>>4)*10 + (value&0x0f);

	DS1302_ReadReg(MIN, &value);
	time->min = ((value&0x70)>>4)*10 + (value&0x0f);

	DS1302_ReadReg(SEC, &value);
	time->sec = ((value&0x70)>>4)*10 + (value&0x0f);
}

void DS1302_SetTime(ds_time_s *time)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(YEAR, ((time->year/10)<<4)|(time->year%10));
	DS1302_WriteReg(MONTH, ((time->month/10)<<4)|(time->month%10));
	DS1302_WriteReg(DATE, ((time->day/10)<<4)|(time->day%10));
	DS1302_WriteReg(HR, ((time->hour/10)<<4)|(time->hour%10));
	DS1302_WriteReg(MIN, ((time->min/10)<<4)|(time->min%10));
	DS1302_WriteReg(SEC, ((time->sec/10)<<4)|(time->sec%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

#define xMINUTE   (60)           // 1分的秒数
#define xHOUR     (60*xMINUTE)   // 1小时的秒数
#define xDAY      (24*xHOUR)     // 1天的秒数
#define xYEAR     (365*xDAY)     // 1年的秒数


const uint8_t monthes[12]={
    /*01月*/31,
    /*02月*/28,
    /*03月*/31,
    /*04月*/30,
    /*05月*/31,
    /*06月*/30,
    /*07月*/31,
    /*08月*/31,
    /*09月*/30,
    /*10月*/31,
    /*11月*/30,
    /*12月*/31
};

static unsigned char applib_dt_is_leap_year(unsigned short year)
{

    if ((year % 400) == 0) {
        return 1;
    } else if ((year % 100) == 0) {
        return 0;
    } else if ((year % 4) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void tick_to_time(uint32_t unix_time, ds_time_s *utc_time)
{
    uint32_t days, leap_y_count;
    utc_time->sec = unix_time%60;   // 获得秒
    unix_time /= 60;
    utc_time->min = unix_time%60;    // 获得分
    unix_time /= 60;
    utc_time->hour = unix_time % 24;   // 获得时
    days = unix_time/24;            // 获得总天数
    leap_y_count = (days + 365)/1461; //过去了多少个闰年(4年一闰)
    if(((days + 366)%1461) == 0)
    {//闰年的最后1天
    	utc_time->year  = (uint8_t)((1970 + ( days/366) ) % 100); // 获得年
    	utc_time->month = 12;                // 调整月
    	utc_time->day = 31;
        return;
    }
    days -= leap_y_count;
    utc_time->year = (uint8_t)((1970 + (days/365) ) % 100);     //获得年
    days %= 365;                                                 //今年的第几天
    days = 01 + days;                                            //1日开始
    if((utc_time->year % 4) == 0)
    {
        if(days > 60){
        	--days;            //闰年调整
        }
        else
        {
            if(days == 60)
            {
            	utc_time->month = 2;
            	utc_time->day = 29;
                return;
            }
        }
    }

    for(utc_time->month= 0; monthes[utc_time->month] < days; utc_time->month++)
    {
        days -= monthes[utc_time->month];
    }
    ++utc_time->month;               //调整月
    utc_time->day = days;           //获得日
}

uint32_t time_to_tick(ds_time_s *utc_time)
{
    uint32_t monthes[12]={
            xDAY*(0),                                           /*01月*/
            xDAY*(31),                                          /*02月*/
            xDAY*(31+28),                                       /*03月*/
            xDAY*(31+28+31),                                    /*04月*/
            xDAY*(31+28+31+30),                                 /*05月*/
            xDAY*(31+28+31+30+31),                              /*06月*/
            xDAY*(31+28+31+30+31+30),                           /*07月*/
            xDAY*(31+28+31+30+31+30+31),                        /*08月*/
            xDAY*(31+28+31+30+31+30+31+31),                     /*09月*/
            xDAY*(31+28+31+30+31+30+31+31+30),                  /*10月*/
            xDAY*(31+28+31+30+31+30+31+31+30+31),               /*11月*/
            xDAY*(31+28+31+30+31+30+31+31+30+31+30)             /*12月*/
  };

  uint32_t seconds = 0;
  uint32_t year = 0;

  year = (utc_time->year + 2000) - 1970;
  seconds = xYEAR*year + xDAY*((year+1)/4);           // 前几年过去的秒数
  seconds += monthes[utc_time->month - 1];             // 加上今年本月过去的秒数

  if( (utc_time->month > 2) && (((year + 2)%4)==0) )   // 2008年为闰年
    seconds += xDAY;                                  // 闰年加1天秒数

  seconds += xDAY*(utc_time->day - 1);                 // 加上本月过去天的秒数
  seconds += xHOUR*utc_time->hour;                     // 加上本天过去小时的秒数
  seconds += xMINUTE*utc_time->min;                    // 加上本小时过去分钟的秒数
  seconds += utc_time->sec;                            // 加上当前秒数

  return seconds;
}

uint32_t tick_Unix_to_Beijing(uint32_t unix_tick)
{
	return unix_tick + 8*xHOUR;
}

uint32_t tick_Beijing_to_Unix(uint32_t beijing_tick)
{
	return beijing_tick - 8*xHOUR;
}

void time_UTC_to_Beijing(ds_time_s *utc_time, ds_time_s *beijing_time)
{
	uint32_t utc_tick = time_to_tick(utc_time);
	tick_to_time(tick_Unix_to_Beijing(utc_tick), beijing_time);
}

void time_Beijing_to_UTC(ds_time_s *beijing_time, ds_time_s *utc_time)
{
	uint32_t beijing_tick = time_to_tick(beijing_time);
	tick_to_time(tick_Beijing_to_Unix(beijing_tick), utc_time);
}

uint32_t DS1302_Read_Unix_Tick(void)
{
  ds_time_s beijing_time;
  DS1302_GetTime(&beijing_time);

  return tick_Beijing_to_Unix(time_to_tick(&beijing_time));
}

void DS1302_Write_Unix_Tick(uint32_t unix_time)
{
  ds_time_s beijing_time;
  uint32_t beijing_tick = tick_Unix_to_Beijing(unix_time);

  tick_to_time(beijing_tick, &beijing_time);
  DS1302_SetTime(&beijing_time);
}

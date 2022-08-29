
#include "ad147a.h"

#include <stdio.h>

#define USE_GPIO_I2C

#ifdef USE_GPIO_I2C
#include "sim_i2c.h"
#else
#include "i2c.h"
#endif

static void AD147A_Write_Reg(uint8_t REG_Address, uint8_t REG_data)
{
#ifdef USE_GPIO_I2C
	if(!sim_i2c_send_byte_data(I2C_2, AD147A_ADDR, REG_Address, REG_data)) {
		printf("L3G4200D_Write_Reg write error\r\n");
	}
#else
	uint8_t rxdata[2] = {REG_Address, REG_data};
    while(HAL_I2C_Master_Transmit(&hi2c1, AD147A_WRITE, rxdata, 2, 5000) != HAL_OK) {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
        	printf("L3G4200D_Write_Reg write error\r\n");
        }
    }
#endif	
}

static uint8_t AD147A_Read_Reg(uint8_t REG_Address)
{	
	uint8_t REG_data;
#ifdef USE_GPIO_I2C
	REG_data = sim_i2c_receive_byte_data(I2C_2, AD147A_ADDR, REG_Address);
	
#else
	while(HAL_I2C_Master_Transmit(&hi2c1, AD147A_WRITE, &REG_Address, 1, 5000) != HAL_OK) {
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
			printf("L3G4200D_Read_Reg write error\r\n");
		}
	}

	if(HAL_I2C_Master_Receive(&hi2c1, AD147A_READ, &REG_data, 1, 5000) != HAL_OK) {
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
			printf("L3G4200D_Read_Reg read error\r\n");
		}
	}
#endif
	return REG_data;	
}

static uint8_t get_id(void)
{
	return AD147A_Read_Reg(AD147A_WHO_AM_I);
}

/* Initialize AD147A Middle Pressure module */
void ad147a_init(void)
{
	printf("AD147A: 0x%02X\r\n", get_id());
	
	//  "10101000"- register action mode, Data rate 100Hz, PMES, TMES 
	AD147A_Write_Reg(AD147A_CTL2, 0xA8);
}

static void printfloat(char *str, float value)
{
	int tmp, tmp1, tmp2, tmp3, tmp4;
	tmp=(int)value;
	tmp1 = (int)((value-tmp)*10)%10;
	tmp2 = (int)((value-tmp)*100)%10;
	tmp3 = (int)((value-tmp)*1000)%10;
	tmp4 = (int)((value-tmp)*10000)%10;
	printf("%s: %d.%d%d%d%d\r\n", str, tmp, tmp1,tmp2,tmp3,tmp4);
}

/* get pressure data */
void ad147a_get_press_temp(float *pressure, float *temp)
{
	uint32_t raw_data;
	uint8_t  status;

	AD147A_Write_Reg(AD147A_ACT_CTRL1, AD147A_ACT_CTRL1_BOTH);
	while(1){
		status = AD147A_Read_Reg(AD147A_STATUS);
		if ((status&AD147A_STAT_CHECK) == AD147A_STAT_BOTH) {
			raw_data = 0;
			raw_data |= AD147A_Read_Reg(AD147A_PRESS_LOW);
			raw_data |= (AD147A_Read_Reg(AD147A_PRESS_MID) << 8);
			raw_data |= (AD147A_Read_Reg(AD147A_PRESS_HIGH) << 8);
			raw_data &= 0x1FF;
			*pressure = raw_data*6.0/1000;
			printfloat("press[kPa] : ", *pressure);


			raw_data = 0;
			raw_data |= AD147A_Read_Reg(AD147A_TEMP_LOW);
			raw_data |= (AD147A_Read_Reg(AD147A_TEMP_HIGH) << 8);
			*temp = (raw_data>>8)*1.0;
			printfloat("temp : ", *temp);
			break;
		}
	}
}


#include "ad14xx.h"

#include <stdio.h>

#define USE_GPIO_I2C

#ifdef USE_GPIO_I2C
#include "sim_i2c.h"
#else
#include "i2c.h"
#endif

typedef struct {
	uint8_t sensor_if;
	uint8_t sensor_addr;
	uint8_t press_coef;
} sensor_param_map_s;

static sensor_param_map_s sensor_param_map[] = {
	{I2C_1, AD146A_ADDR, AD146A_PRESS_COEF},        // AD146A_HIGH,
	{I2C_2, AD147A_ADDR, AD147A_PRESS_COEF},        // AD147A_MID,
	{I2C_3, AD143C_ADDR, AD143C_PRESS_COEF}         // AD143C_LOW,
};

static void AD14XX_Write_Reg(ad14xx_type_e type, uint8_t REG_Address, uint8_t REG_data)
{
#ifdef USE_GPIO_I2C
	if(!sim_i2c_send_byte_data( sensor_param_map[type].sensor_if,
			                    sensor_param_map[type].sensor_addr,
			                    REG_Address,
								REG_data)) {
		printf("AD14XX_Write_Reg write error\r\n");
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

static uint8_t AD14XX_Read_Reg(ad14xx_type_e type, uint8_t REG_Address)
{
	uint8_t REG_data;
#ifdef USE_GPIO_I2C
	REG_data = sim_i2c_receive_byte_data( sensor_param_map[type].sensor_if,
                                          sensor_param_map[type].sensor_addr,
			                              REG_Address);

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

static uint8_t get_id(ad14xx_type_e type)
{
	return AD14XX_Read_Reg(type, AD14XX_WHO_AM_I);
}

/* Initialize AD14XX Pressure module */
void ad14xx_init(ad14xx_type_e type)
{
	switch (type) {
		case AD146A_HIGH:
			printf("AD146A: 0x%02X\r\n", get_id(type));
			break;
		case AD147A_MID:
			printf("AD147A: 0x%02X\r\n", get_id(type));
			break;
		default:
			printf("AD143C: 0x%02X\r\n", get_id(type));
			break;
	}

	//  "10101000"- register action mode, Data rate 100Hz, PMES, TMES
	AD14XX_Write_Reg(type, AD14XX_CTL2, 0xA8);
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
void ad14xx_get_press_temp(ad14xx_type_e type, float *pressure, float *temp)
{
	uint32_t raw_data;
	uint8_t  status;

	AD14XX_Write_Reg(type, AD14XX_ACT_CTRL1, AD14XX_ACT_CTRL1_BOTH);
	while(1){
		status = AD14XX_Read_Reg(type, AD14XX_STATUS);
		if ((status&AD14XX_STAT_CHECK) == AD14XX_STAT_BOTH) {
			raw_data = 0;
			raw_data |=  AD14XX_Read_Reg(type, AD14XX_PRESS_LOW);
			raw_data |= (AD14XX_Read_Reg(type, AD14XX_PRESS_MID) << 8);
			raw_data |= (AD14XX_Read_Reg(type, AD14XX_PRESS_HIGH) << 8);
			raw_data &= 0x1FF;
			*pressure = raw_data*sensor_param_map[type].press_coef*1.0/1000;
			printfloat("press[kPa] : ", *pressure);


//			raw_data = 0;
//			raw_data |= AD14XX_Read_Reg(type, AD14XX_TEMP_LOW);
//			raw_data |= (AD14XX_Read_Reg(type, AD14XX_TEMP_HIGH) << 8);
//			*temp = (raw_data>>8)*1.0;
			raw_data = AD14XX_Read_Reg(type, AD14XX_TEMP_HIGH);
			*temp = raw_data*1.0;
			printfloat("temp : ", *temp);
			break;
		}
	}
}

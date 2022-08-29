
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

#if 0
/* get X-axis data */
int16_t L3G4200D_Get_X(void)
{
	uint8_t buf[2];
	int16_t data;

    buf[0]= L3G4200D_Read_Reg(L3G4200D_OUT_X_L);
    buf[1]= L3G4200D_Read_Reg(L3G4200D_OUT_X_H);

    data=(buf[1]<<8)+buf[0];

    return data;
}

/* get Y-axis data */
int16_t L3G4200D_Get_Y(void)
{
	uint8_t buf[2];
	int16_t data;

    buf[0]= L3G4200D_Read_Reg(L3G4200D_OUT_Y_L);
    buf[1]= L3G4200D_Read_Reg(L3G4200D_OUT_Y_H);

    data =( buf[1] << 8) + buf[0];

    return data;
}


/* get Z-axis data */
int16_t L3G4200D_Get_Z(void)
{
	uint8_t buf[2];
	int16_t data;

    buf[0]= L3G4200D_Read_Reg(L3G4200D_OUT_Z_L);
    buf[1]= L3G4200D_Read_Reg(L3G4200D_OUT_Z_H);

    data=(buf[1]<<8)+buf[0];

    return data;
}


#endif


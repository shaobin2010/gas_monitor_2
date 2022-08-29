
#include "../press_temp_sensor/ad147a.h"

#include <stdio.h>
#include "sim_i2c.h"

static void AD147A_Write_Reg(uint8_t REG_Address, uint8_t REG_data)
{
	if(!sim_i2c_send_byte_data(I2C_2, AD147A_ADDR, REG_Address, REG_data)) {
		printf("L3G4200D_Write_Reg write error\r\n");
	}
}

static uint8_t AD147A_Read_Reg(uint8_t REG_Address)
{
	return sim_i2c_receive_byte_data(I2C_2, AD147A_ADDR, REG_Address);
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


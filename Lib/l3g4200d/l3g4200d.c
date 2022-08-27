
#define USE_GPIO_I2C

#ifdef USE_GPIO_I2C
#include "sim_i2c.h"
#else
#include "i2c.h"
#endif

#include "l3g4200d.h"

#include <stdio.h>

static void L3G4200D_Write_Reg(uint8_t REG_Address, uint8_t REG_data)
{
#ifdef USE_GPIO_I2C
	if(!sim_i2c_send_byte_data(I2C_L3G, L3G4200D_ADDR, REG_Address, REG_data)) {
		printf("L3G4200D_Write_Reg write error\r\n");
	}
#else
	uint8_t rxdata[2] = {REG_Address, REG_data};
    while(HAL_I2C_Master_Transmit(&hi2c1, L3G4200D_WRITE, rxdata, 2, 5000) != HAL_OK) {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
        	printf("L3G4200D_Write_Reg write error\r\n");
        }
    }
#endif
}

static uint8_t L3G4200D_Read_Reg(uint8_t REG_Address)
{
	uint8_t REG_data;
#ifdef USE_GPIO_I2C
	REG_data = sim_i2c_receive_byte_data(I2C_L3G, L3G4200D_ADDR, REG_Address);
	
#else
	while(HAL_I2C_Master_Transmit(&hi2c1, L3G4200D_WRITE, &REG_Address, 1, 5000) != HAL_OK) {
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
			printf("L3G4200D_Read_Reg write error\r\n");
		}
	}

	if(HAL_I2C_Master_Receive(&hi2c1, L3G4200D_READ, &REG_data, 1, 5000) != HAL_OK) {
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
			printf("L3G4200D_Read_Reg read error\r\n");
		}
	}
#endif
	return REG_data;
}

static uint8_t get_id(void)
{
	uint8_t byte;
	byte = L3G4200D_Read_Reg(L3G4200D_WHO_AM_I);

	return byte;
}

/* Initialize L3G4200D module */
void L3G4200D_Init(void)
{
	printf("L3G3200D: 0x%02X\r\n", get_id());
	//  "00111111"- Data rate 100Hz, BW 25Hz, power up, enable all axes

	L3G4200D_Write_Reg(L3G4200D_CTRL_REG1, 0x3F);
//	L3G4200D_Write_Reg(L3G4200D_CTRL_REG1, 0xEF);   // 800Hz
	L3G4200D_Write_Reg(L3G4200D_CTRL_REG2, 0x00);   //
	L3G4200D_Write_Reg(L3G4200D_CTRL_REG3, 0x00);   //
	L3G4200D_Write_Reg(L3G4200D_CTRL_REG4, 0x00);  //+-250dps
	L3G4200D_Write_Reg(L3G4200D_CTRL_REG5, 0x00);
#if 0
	printf("L3G4200D_CTRL_REG1: 0x%02X\r\n", L3G4200D_Read_Reg(L3G4200D_CTRL_REG1));
	printf("L3G4200D_CTRL_REG2: 0x%02X\r\n", L3G4200D_Read_Reg(L3G4200D_CTRL_REG2));
	printf("L3G4200D_CTRL_REG3: 0x%02X\r\n", L3G4200D_Read_Reg(L3G4200D_CTRL_REG3));
	printf("L3G4200D_CTRL_REG4: 0x%02X\r\n", L3G4200D_Read_Reg(L3G4200D_CTRL_REG4));
	printf("L3G4200D_CTRL_REG5: 0x%02X\r\n", L3G4200D_Read_Reg(L3G4200D_CTRL_REG5));
#endif
}

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


#if 0
uint8_t L3G4200D_WhoAmI(void)
{
//	sim_i2c_init(I2C_L3G);
	HAL_I2C_GetState();
    // should be 0xD3
    return sim_i2c_receive_byte_data(I2C_L3G, L3G4200D_ADDR, L3G4200D_WHO_AM_I);
}


void L3G4200D_Setup(void)
{
    i2c_XmtByte(I2C_L3G, L3G4200D_ADDR, L3G4200D_CTRL_REG1, 0x3e); //
	i2c_XmtByte(I2C_L3G, L3G4200D_ADDR, L3G4200D_CTRL_REG4, 0x90); // "10010000"  500dps, disable selftest
}

void L3G4200D_ReadRaw(int16_t *pxraw, int16_t *pyraw, int16_t* pzraw) 
{
	uint8_t buf[6];
	int16_t x, y, z;
	i2c_RcvBuf(I2C_L3G, L3G4200D_ADDR, L3G4200D_OUT_X_L | (1 << 7), 6, buf); //read the acceleration data from the L3G4200D
	// each axis reading comes in 13 bit 2's complement format. lsb first, msb has sign bits extended
	x = (int16_t) ((((uint16_t) buf[1]) << 8) | (uint16_t) buf[0]);
	*pxraw = x;
	y = (int16_t) ((((uint16_t) buf[3]) << 8) | (uint16_t) buf[2]);
	*pyraw = y;
	z = (int16_t) ((((uint16_t) buf[5]) << 8) | (uint16_t) buf[4]);
	*pzraw = z;
}
#endif

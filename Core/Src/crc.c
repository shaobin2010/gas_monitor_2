/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.c
  * @brief   This file provides code for the configuration
  *          of the CRC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "crc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* CRC init function */
void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/* USER CODE BEGIN 1 */
void hw_crc32_init(void)
{
	LL_CRC_ResetCRCCalculationUnit(CRC);
}

uint32_t hw_crc32_feeddata(uint32_t *buff, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		LL_CRC_FeedData32(CRC, buff[i]);
	}
	return LL_CRC_ReadData32(CRC);
}

#if 0
uint32_t sw_crc32_feeddata(uint32_t last_crc, uint32_t *buff, uint32_t size)
{
	uint32_t polyNomial = 0x04C11DB7, crc32;
	uint32_t data, xbit, bits;
	crc32 = last_crc;

	while(size--) {
		xbit = 1 << 31;
		data = *buff++;
		for(bits = 0; bits < 32; bits++){
			if(crc32 & 0x80000000) {
				crc32 <<= 1;
				crc32 ^= polyNomial;
			} else {
				crc32 <<= 1;
			}

			if(data &xbit) {
				crc32 ^= polyNomial;
			}

			xbit >>= 1;
		}
	}

	return crc32;
}


//static const uint32_t test_data[] = {
//0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
//0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
//0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
//0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91
//};

static const uint8_t test_data[] = {
0x00, 0x00, 0x00, 0x00, 0x77, 0x07, 0x30, 0x96,
0xee, 0x0e, 0x61, 0x2c, 0x99, 0x09, 0x51, 0xba,
0x07, 0x6d, 0xc4, 0x19, 0x70, 0x6a, 0xf4, 0x8f,
0xe9, 0x63, 0xa5, 0x35, 0x9e, 0x64, 0x95, 0xa3,
0x0e, 0xdb, 0x88, 0x32, 0x79, 0xdc, 0xb8, 0xa4,
0xe0, 0xd5, 0xe9, 0x1e, 0x97, 0xd2, 0xd9, 0x88,
0x09, 0xb6, 0x4c, 0x2b, 0x7e, 0xb1, 0x7c, 0xbd,
0xe7, 0xb8, 0x2d, 0x07, 0x90, 0xbf, 0x1d, 0x91
};


void crc32_test(void)
{
	hw_crc32_init();
	Debug_Printf_hex(hw_crc32_feeddata((uint32_t *)test_data, 16), "\r\n");
	Debug_Printf_hex(sw_crc32_feeddata(0xFFFFFFFF, (uint32_t *)test_data, 16), "\r\n");
}
#endif
/* USER CODE END 1 */

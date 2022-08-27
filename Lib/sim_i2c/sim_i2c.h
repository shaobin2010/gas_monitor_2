#ifndef __SIM_I2C_H__
#define __SIM_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "main.h"

typedef enum {
	I2C_L3G,
	I2C_1,
	I2C_2,
	I2C_3,
	I2C_NUM
} sim_i2c_id_e;

extern void sim_i2c_init(sim_i2c_id_e id);
extern bool sim_i2c_send_byte_data(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t reg, uint8_t data);
extern uint8_t sim_i2c_receive_byte_data(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t reg);

#ifdef __cplusplus
}
#endif
#endif /*__ __SIM_I2C_H__ */


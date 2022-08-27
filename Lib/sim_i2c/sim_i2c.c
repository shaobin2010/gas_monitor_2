
#include "main.h"
#include "gpio.h"
#include "sim_i2c.h"

extern void board_DelayUs(int32_t udelay);

#define I2C_SCL_LOW(port, pin)    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define I2C_SCL_HIGH(port, pin)   HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define I2C_SDA_LOW(port, pin)    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define I2C_SDA_HIGH(port, pin)   HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define I2C_READ_SDA(port, pin)   HAL_GPIO_ReadPin(port, pin)

static GPIO_TypeDef *scl_port, *sda_port;
static uint16_t scl_pin, sda_pin;

static void setup_i2c_pin(sim_i2c_id_e id)
{
	switch (id) {
	case I2C_L3G:
		sda_port = L3G_SDA_GPIO_Port;
		scl_port = L3G_SCL_GPIO_Port;
		sda_pin  = L3G_SDA_Pin;
		scl_pin  = L3G_SCL_Pin;
		break;

	case I2C_1:
		sda_port = SDA_1_GPIO_Port;
		scl_port = SCL_1_GPIO_Port;
		sda_pin  = SDA_1_Pin;
		scl_pin  = SCL_1_Pin;
		break;

	case I2C_2:
		sda_port = SDA_2_GPIO_Port;
		scl_port = SCL_2_GPIO_Port;
		sda_pin  = SDA_2_Pin;
		scl_pin  = SCL_2_Pin;
		break;

	case I2C_3:
		sda_port = SDA_3_GPIO_Port;
		scl_port = SCL_3_GPIO_Port;
		sda_pin  = SDA_3_Pin;
		scl_pin  = SCL_3_Pin;
		break;

	default:
		break;
	}
}

static void sim_i2c_start(void)
{
	I2C_SDA_HIGH(sda_port, sda_pin);
	I2C_SCL_HIGH(scl_port, scl_pin);
	board_DelayUs(4);

	I2C_SDA_LOW(sda_port, sda_pin);
	board_DelayUs(4);
	I2C_SCL_LOW(scl_port, scl_pin);
	board_DelayUs(4);
}

static void sim_i2c_stop(void)
{
	I2C_SCL_LOW(scl_port, scl_pin);
	I2C_SDA_LOW(sda_port, sda_pin);
	board_DelayUs(4);
	I2C_SCL_HIGH(scl_port, scl_pin);
	I2C_SDA_HIGH(sda_port, sda_pin);
	board_DelayUs(4);
}


static bool sim_i2c_wait_ack(void)
{
	uint8_t ucErrTime = 0;
	I2C_SDA_HIGH(sda_port, sda_pin);
	board_DelayUs(4);
	I2C_SCL_HIGH(scl_port, scl_pin);
	board_DelayUs(4);
	while (I2C_READ_SDA(sda_port, sda_pin)) {

		ucErrTime++;
		if (ucErrTime > 250) {

			sim_i2c_stop();
			return false;
		}
	}
	I2C_SCL_LOW(scl_port, scl_pin);      // Clock output 0
	return true;
}

// produce ACK The reply
static void sim_i2c_send_ack(void)
{
	I2C_SCL_LOW(scl_port, scl_pin);
	I2C_SDA_LOW(sda_port, sda_pin);
	board_DelayUs(4);
	I2C_SCL_HIGH(scl_port, scl_pin);
	board_DelayUs(4);
	I2C_SCL_LOW(scl_port, scl_pin);
}


static void sim_i2c_send_nack(void)
{
	I2C_SCL_LOW(scl_port, scl_pin);
	I2C_SDA_HIGH(sda_port, sda_pin);
	board_DelayUs(4);
	I2C_SCL_HIGH(scl_port, scl_pin);
	board_DelayUs(4);
	I2C_SCL_LOW(scl_port, scl_pin);
}


static void sim_i2c_send_byte(uint8_t txd)
{

	uint8_t t;
	I2C_SCL_LOW(scl_port, scl_pin);      // Pull down the clock and start data transmission
	for (t = 0; t < 8; t++) {
		if(txd & 0x80) {
			I2C_SDA_HIGH(sda_port, sda_pin);
		} else {
			I2C_SDA_LOW(sda_port, sda_pin);
		}
		txd <<= 1;

		board_DelayUs(4);   // Yes TEA5767 All three delays are necessary
		I2C_SCL_HIGH(scl_port, scl_pin);
		board_DelayUs(4);
		I2C_SCL_LOW(scl_port, scl_pin);
		board_DelayUs(4);
	}
}

static uint8_t sim_i2c_read_byte(uint8_t ack)
{

	uint8_t i, receive = 0;
	for (i = 0; i < 8; i++) {

		I2C_SCL_LOW(scl_port, scl_pin);
		board_DelayUs(4);
		I2C_SCL_HIGH(scl_port, scl_pin);
		receive <<= 1;
		if (I2C_READ_SDA(sda_port, sda_pin))
			receive++;
		board_DelayUs(4);
	}

	if (!ack)
		sim_i2c_send_nack();   // send out nACK
	else
		sim_i2c_send_ack(); // send out ACK
	return receive;
}

/*
static void sim_i2c_write_bit(uint8_t b)
{
    if(b > 0) {
        I2C_SDA_HIGH(sda_port, sda_pin);
    } else {
        I2C_SDA_LOW(sda_port, sda_pin);
    }

    board_DelayUs(100);
	I2C_SCL_HIGH(scl_port, scl_pin);
	board_DelayUs(100);
    I2C_SCL_LOW(scl_port, scl_pin);
    board_DelayUs(100);
}

static uint8_t sim_i2c_read_SDA(void)
{
    if(I2C_READ_SDA(sda_port, sda_pin) == GPIO_PIN_SET) {
        return 1;
    } else {
        return 0;
    }
}

static uint8_t sim_i2c_read_bit(void)
{
    uint8_t b;

    I2C_SDA_HIGH(sda_port, sda_pin);
    board_DelayUs(100);
	I2C_SCL_HIGH(scl_port, scl_pin);
	board_DelayUs(100);

    b = sim_i2c_read_SDA();
    I2C_SCL_LOW(scl_port, scl_pin);
    return b;
}


static bool sim_i2c_write_byte(uint8_t B, bool start, bool stop)
{
    uint8_t i;
    uint8_t ack = 0;

    if (start) {
        sim_i2c_start();
    }

    for (i = 0; i < 8; i++) {
        sim_i2c_write_bit(B & 0x80);
        B <<= 1;
    }

    ack = sim_i2c_read_bit();

    if(stop) {
        sim_i2c_stop();
    }

    return ack ? false : true;
}

static uint8_t sim_i2c_read_byte(bool ack, bool stop)
{
    uint8_t i;
    uint8_t B = 0;

    for (i = 0; i < 8; i++) {
        B <<= 1;
        B |= sim_i2c_read_bit();
    }

    if (ack) {
        sim_i2c_write_bit(0);
    } else {
        sim_i2c_write_bit(1);
    }

    if (stop) {
        sim_i2c_stop();
    }

    return B;
}

*/

#if 0
uint8_t sim_i2c_send_byte(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t data)
{
    // set global port/pin
	setup_i2c_pin(id);

    // start + i2c_add + write
    if(sim_i2c_write_byte((i2c_addr << 1), 1, 0)) {
        // send data, stop
        if(sim_i2c_write_byte(data, 0, 1)) {
            return 1;
        }
    }

    sim_i2c_stop();
    return 0;
}

uint8_t sim_i2c_receive_byte(sim_i2c_id_e id, uint8_t i2c_addr)
{
    // set global port/pin
	setup_i2c_pin(id);

    // start + i2c_add + read
    if(sim_i2c_write_byte((i2c_addr << 1) | 0x01, 1, 0)) {
        return sim_i2c_read_byte(0, 1);
    }

    return 0;
}
#endif

void sim_i2c_init(sim_i2c_id_e id)
{
    // set global port/pin
	setup_i2c_pin(id);
	sim_i2c_stop();
}


bool sim_i2c_send_byte_data(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t reg, uint8_t data)
{
    // set global port/pin
	setup_i2c_pin(id);
	sim_i2c_start();  //1

	sim_i2c_send_byte(i2c_addr << 1);	    // Send write command 		2
	sim_i2c_wait_ack();	    //3
	sim_i2c_send_byte(reg);	    // Sending address 	4
	sim_i2c_wait_ack();	   	 	//5
	sim_i2c_send_byte(data);     // Send byte 				6
	sim_i2c_wait_ack();  	//7
	sim_i2c_stop();  	// Create a stop condition  	8
	board_DelayUs(100);
	return true;
}

uint8_t sim_i2c_receive_byte_data(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t reg)
{
	uint8_t data;
    // set global port/pin
	setup_i2c_pin(id);
	sim_i2c_start();  //1

	sim_i2c_send_byte(i2c_addr << 1);	   // Send write command 	2
	sim_i2c_wait_ack();	   //3
	sim_i2c_send_byte(reg);	   // Sending address 	4
	sim_i2c_wait_ack();		 //5

	sim_i2c_start();  	 	//6
	sim_i2c_send_byte((i2c_addr << 1) | 0x01);  	 	// Enter receive mode 		7
	sim_i2c_wait_ack();	// 8
	data = sim_i2c_read_byte(0);		   //9 10
	board_DelayUs(100);	   // Create a stop condition 	    //11
	return data;
}


/*
bool sim_i2c_send_byte_data(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t reg, uint8_t data)
{
    // set global port/pin
	setup_i2c_pin(id);

    // start + i2c_add + write
    if(sim_i2c_write_byte((i2c_addr << 1), true, false)) {
        // write register
        if(sim_i2c_write_byte(reg, false, false)) {
           if(sim_i2c_write_byte(data, false, true)) {
                return true;
           }
        }
    }

    sim_i2c_stop();
    return false;
}

uint8_t sim_i2c_receive_byte_data(sim_i2c_id_e id, uint8_t i2c_addr, uint8_t reg)
{
    // set global port/pin
	setup_i2c_pin(id);

    // start + i2c_add + write
    if(sim_i2c_write_byte((i2c_addr << 1), true, false)) {
        // write register
        if(sim_i2c_write_byte(reg, false, false)) {
            // start again, send i2c_addr, read
           if(sim_i2c_write_byte((i2c_addr << 1) | 0x01, true, false)) {
                return sim_i2c_read_byte(false, true);
           }
        }
    }

    sim_i2c_stop();
    return 0;
}

*/

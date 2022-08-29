#ifndef __AD147A_MID_H__
#define __AD147A_MID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

#define AD147A_ADDR  0x48
#define AD147A_WRITE 0x90
#define AD147A_READ  0x91

#define AD147A_WHO_AM_I            0x00      // should be 0x49
#define AD147A_INFO                0x01      // should be 0x31
#define AD147A_FIFO_STATUS         0x02

#define AD147A_STAT_PRDY           (1 << 0)  // pressure data ready
#define AD147A_STAT_PDOR           (1 << 2)  // data over fun flag
#define AD147A_STAT_TRDY           (1 << 4)  // temp data ready
#define AD147A_STAT_BUSY           (1 << 7)  // busy state flag
#define AD147A_STATUS              0x03

// [16:0]  17 bit pressure output 0 ~ 131071 Pa = POUT[LSB] * 6
#define AD147A_PRESS_LOW           0x04
#define AD147A_PRESS_MID           0x05
#define AD147A_PRESS_HIGH          0x06

// [15:0]  16 bit pressure output -32768 ~ 32768 C = POUT[LSB] / 256
#define AD147A_TEMP_LOW            0x09
#define AD147A_TEMP_HIGH           0x0A

#define AD147A_CTL1_PTAP           (0x3 << 0)  // sinc filter Tap count
#define AD147A_CTL1                0x0E

#define AD147A_CTL2_MODE           (0x3 << 0)  // work mode
#define AD147A_CTL2_ODR            (0x3 << 2)  // sample rate
#define AD147A_CTL2_PMES           (  1 << 5)  // Pressure measurement in continuous mode
#define AD147A_CTL2_TMES           (  1 << 7)  // Temperature measurement in continuous mode
#define AD147A_CTL2                0x0F

#define AD147A_ACT_CTRL1_PDET      (1 << 1)
#define AD147A_ACT_CTRL1_TDET      (1 << 3)
#define AD147A_ACT_CTRL1           0x10   // register action mode/ command action mode

#define AD147A_ACT_CTRL2_SRST     (1 << 7)
#define AD147A_ACT_CTRL2           0x11   // command action mode

#define AD147A_FIFO_CTRL           0x12
#define AD147A_AVG_CTRL            0x13
#define AD147A_PROD_NUM            0x1C   // should be 0x31

#define AD147A_PDET                0x20   // command action mode
#define AD147A_TDET                0x22   // command action mode
#define AD147A_RESET               0x26   // command action mode
#define AD147A_PTDET               0x29   // command action mode


extern void ad147a_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__AD147A_MID_H__ */

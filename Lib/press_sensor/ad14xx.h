/*
 * AD146A:               30kPa to 1700kPa        (+9000m or 150m depth)
 * AD147A:               30kPa to 750kPa         (+9000m or 60m depth)
 * AD143C:               300hPa to 2000hPa       1kPa = 10hPa
 */

#ifndef __AD14XX_H__
#define __AD14XX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

// 高压
#define AD146A_ADDR  0x49
#define AD146A_WRITE 0x92
#define AD146A_READ  0x93
// 中压
#define AD147A_ADDR  0x48
#define AD147A_WRITE 0x90
#define AD147A_READ  0x91
// 低压
#define AD143C_ADDR  0x48
#define AD143C_WRITE 0x90
#define AD143C_READ  0x91

#define AD14XX_WHO_AM_I            0x00      // should be 0x49
#define AD14XX_INFO                0x01      // should be 0x31
#define AD14XX_FIFO_STATUS         0x02

#define AD14XX_STAT_PRDY           (1 << 0)  // pressure data ready
#define AD14XX_STAT_PDOR           (1 << 2)  // data over fun flag
#define AD14XX_STAT_TRDY           (1 << 4)  // temp data ready
#define AD14XX_STAT_BUSY           (1 << 7)  // busy state flag
#define AD14XX_STAT_BOTH           (AD14XX_STAT_PRDY|AD14XX_STAT_TRDY)
#define AD14XX_STAT_CHECK          (AD14XX_STAT_BUSY|AD14XX_STAT_BOTH)
#define AD14XX_STATUS              0x03

// [16:0]  17 bit pressure output 0 ~ 131071 Pa = POUT[LSB] * 6
#define AD14XX_PRESS_LOW           0x04
#define AD14XX_PRESS_MID           0x05
#define AD14XX_PRESS_HIGH          0x06

#define AD146A_PRESS_COEF          13
#define AD147A_PRESS_COEF          6
#define AD143C_PRESS_COEF          2


// [15:0]  16 bit pressure output -32768 ~ 32768 C = POUT[LSB] / 256
#define AD14XX_TEMP_LOW            0x09
#define AD14XX_TEMP_HIGH           0x0A
#define AD14XX_TEMP_COEF           256

#define AD14XX_CTL1_PTAP           (0x3 << 0)  // sinc filter Tap count
#define AD14XX_CTL1                0x0E

#define AD14XX_CTL2_MODE           (0x3 << 0)  // work mode
#define AD14XX_CTL2_ODR            (0x3 << 2)  // sample rate
#define AD14XX_CTL2_PMES           (  1 << 5)  // Pressure measurement in continuous mode
#define AD14XX_CTL2_TMES           (  1 << 7)  // Temperature measurement in continuous mode
#define AD14XX_CTL2                0x0F

#define AD14XX_ACT_CTRL1_PDET      (1 << 1)
#define AD14XX_ACT_CTRL1_TDET      (1 << 3)
#define AD14XX_ACT_CTRL1_BOTH      (AD14XX_ACT_CTRL1_PDET | AD14XX_ACT_CTRL1_TDET)
#define AD14XX_ACT_CTRL1           0x10   // register action mode/ command action mode

#define AD14XX_ACT_CTRL2_SRST     (1 << 7)
#define AD14XX_ACT_CTRL2           0x11   // command action mode

#define AD14XX_FIFO_CTRL           0x12
#define AD14XX_AVG_CTRL            0x13
#define AD14XX_PROD_NUM            0x1C   // should be 0x31

#define AD14XX_PDET                0x20   // command action mode
#define AD14XX_TDET                0x22   // command action mode
#define AD14XX_RESET               0x26   // command action mode
#define AD14XX_PTDET               0x29   // command action mode

typedef enum {
    AD146A_HIGH,       
    AD147A_MID,       
    AD143C_LOW,
} ad14xx_type_e;


extern void ad14xx_init(ad14xx_type_e type);
extern void ad14xx_get_press_temp(ad14xx_type_e type, float *pressure, float *temp);

#ifdef __cplusplus
}
#endif
#endif /*__AD14XX_H__ */

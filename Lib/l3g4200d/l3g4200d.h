#ifndef __L3G4200D_H__
#define __L3G4200D_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define L3G4200D_ADDR  0x69
#define L3G4200D_WRITE 0xD2
#define L3G4200D_READ  0xD3

#define L3G4200D_WHO_AM_I            0x0F
#define L3G4200D_CTRL_REG1           0x20
#define L3G4200D_CTRL_REG2           0x21
#define L3G4200D_CTRL_REG3           0x22
#define L3G4200D_CTRL_REG4           0x23
#define L3G4200D_CTRL_REG5           0x24
#define L3G4200D_REFERENCE           0x25
#define L3G4200D_OUT_TEMP            0x26
#define L3G4200D_STATUS_REG          0x27
#define L3G4200D_OUT_X_L             0x28
#define L3G4200D_OUT_X_H             0x29
#define L3G4200D_OUT_Y_L             0x2A
#define L3G4200D_OUT_Y_H             0x2B
#define L3G4200D_OUT_Z_L             0x2C
#define L3G4200D_OUT_Z_H             0x2D
#define L3G4200D_FIFO_CTRL_REG       0x2E
#define L3G4200D_FIFO_SRC_REG        0x2F
#define L3G4200D_INT1_CFG            0x30
#define L3G4200D_INT1_SRC            0x31
#define L3G4200D_INT1_TSH_XH         0x32
#define L3G4200D_INT1_TSH_XL         0x33
#define L3G4200D_INT1_TSH_YH         0x34
#define L3G4200D_INT1_TSH_YL         0x35
#define L3G4200D_INT1_TSH_ZH         0x36
#define L3G4200D_INT1_TSH_ZL         0x37
#define L3G4200D_INT1_DURATION       0x38


extern void L3G4200D_Init(void);
extern int16_t L3G4200D_Get_X(void);
extern int16_t L3G4200D_Get_Y(void);
extern int16_t L3G4200D_Get_Z(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __L3G4200D_H__ */


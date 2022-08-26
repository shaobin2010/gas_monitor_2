#ifndef __RANQI_DEV_H__
#define __RANQI_DEV_H__

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************/
/*                 Device resource                   */
/*****************************************************/

// 双口产品
#define PRODUCT_PORTS       1


typedef enum {
    Ranqi_Port_0   = 0,
#if PRODUCT_PORTS == 2
    Ranqi_Port_1   = 1,
#endif 
    Ranqi_Port_Num
} ranqi_port_e;

typedef enum {
    Ranqi_Sensor_Loc_Low   = 0,
    Ranqi_Sensor_Loc_Mid   = 1,
    Ranqi_Sensor_Loc_High  = 2,
    Ranqi_Sensor_Loc_Num
} Ranqi_Sensor_Loc_e;

#define RANQI_SENSOR_PORT_GET(loc)  (loc >> 4)
#define RANQI_SENSOR_POS_GET(loc)   (loc&0x0F)

/*
采集点目前分为:  
- 次高压   : X2,  
- 中压     : X1
- 低压     : X0

X定义如下：
1----双口产品入口（主传感器、主设备端传感器）
0----双口产品出口（辅传感器、引出传感器）

单口产品： X为0
*/

// SERVER information
#define Ranqi_Default_Server_Ip       0xDDE5D6CA  // 221.229.214.202
#define Ranqi_Default_Server_Port     5683

// Device Information
#define Ranqi_Dev_Hw_Ver              "01"
#define Ranqi_Dev_Sw_Ver              "01"
#define Ranqi_Dev_Vendor_Id           0x00000006
#define Ranqi_Dev_SN                  0x12345678
#define Rqnqi_Dev_Date                0x220819

// Device Component Information
#define Ranqi_CPU_Vendor              1
#define Ranqi_CPU_Model              "stm32f103"
#define Ranqi_NB_Vendor               2
#define Ranqi_NB_Model                "mn316"
#define Ranqi_BT_Vendor               3
#define Ranqi_BT_Model                "null"
#define Ranqi_BT_Vol                  1.1f

// Fang Bao, Fang Shui Information
#define Fang_Bao_Cert_SN              "1111111"   // 防爆证书编号
#define Fang_Bao_Level                "2222222"   // 防爆等级
#define Fang_Shui_Level               "3333333"   // 防水等级

#define DEFAULT_SAMPLE_INTERVAL        5
#define DEFAULT_REPORT_INTERVAL        60

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} tilt_angle_s;

typedef struct {
    uint8_t device_port;        // 1 or 2 ports
    uint32_t server_ip;
    uint16_t server_port;

    // sample & report interval
    uint8_t  sample_interval;   // >=1, default 5分钟
    uint8_t  report_interval;   // >=1, default 60分钟
    tilt_angle_s  init_tilt_angle;

    // System parameters related
    uint8_t  alarm_tolerance;
    uint8_t  filter_coef;
    uint8_t  report_start_limit;
    uint8_t  report_start_kuang;

    // Alarm parameters related
#if PRODUCT_PORTS == 2
    float   over_press_dn_limit[3][2];  // loc, port
    float   over_press_up_limit[3][2];  // loc, port
    float   under_press_up_limit[3][2]; // loc, port
    float   under_press_dn_limit[3][2]; // loc, port
    float   temp_dn_limit[3][2];        // loc, port
    float   temp_up_limit[3][2];        // loc, port

    float   zero_shift_adjust[3][2][2]; // loc, type, port
    float   xie_shift_adjust[3][2][2];  // loc, type, port
#else
    float   over_press_up_limit[3][1];  // loc, port
    float   over_press_dn_limit[3][1];  // loc, port
    float   under_press_up_limit[3][1]; // loc, port
    float   under_press_dn_limit[3][1];  // loc, port
    float   temp_dn_limit[3][1];         // loc, port   >= -20°
    float   temp_up_limit[3][1];         // loc, port   <= 60°

    float   zero_shift_adjust[3][2][1];  // loc, type, port
    float   xie_shift_adjust[3][2][1];   // loc, type, port
#endif
    uint16_t battry_low_limit;
    float   tilt_limit;       // 一个位置， 采样点： 0x03  <= 65°
    float   crash_acc_limit;  // 一个位置， 采样点： 0x06


    uint32_t alarm_win;         // 3 次; 连续3次超过报警阈值时，每次20s
    uint32_t filter_num;        // 1;   多次采集数据，取平均值上报，该次数为滤波系数

    uint32_t ts;                // 采集时间对其到00秒
    uint32_t up_pkg_id;        // 0x00000000 ~ 0x7FFFFFFF
    uint32_t dn_pkg_id;        // 0x80000000 ~ 0xFFFFFFFF
} ranqi_device_s;


extern void ranqi_dev_init(void);
extern char *ranqi_get_imei(void);

#ifdef __cplusplus
}
#endif

#endif /* !__RANQI_DEV_H__ */

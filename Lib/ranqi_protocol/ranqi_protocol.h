
#include <stdio.h>
#include <stdint.h>

#include "ranqi_dev.h"

/*********************************************************/
/*                  up & down                            */
/********************************************************/
typedef enum {
    MSG_ALARM                  = 0x11,        // 报警数据           Up
    MSG_NORNAL                 = 0x22,        // 正常数据           Up
    MSG_FEEDBACK               = 0x33,        // 信息反馈           Up
    MSG_ACK                    = 0x33,        // 回馈消息           Dn
    MSG_MUNUAL                 = 0x44,        // 手动上传           Up
    MSG_ALM_PARAM_SET          = 0x55,        // 设置报警参数指令    Dn
    MSG_SYS_PARAM_SET          = 0x66,        // 设置10参数指令      Dn
    MSG_INTERVAL               = 0x68,        // 设置数据采集，上传时间间隔指令      Dn
    MSG_TIME                   = 0x77,        // 授时指令            Dn
    MSG_REQUEST_REPORT         = 0x88,        // 要求上传参数        Dn
    MSG_SHIFT_ADJUST           = 0x99,        // 零漂斜漂修正        Dn
    MSG_INIT_TILT_ANGLE        = 0x51,        // 倾斜初始角          Dn
    MSG_SERVER_INFO            = 0x67,        // 服务器信息          Dn
    MSG_SLEEP                  = 0x58,        // 进入休眠            Dn
    MSG_MORE                   = 0xCC,        // 补发数据            Up
    
    MSG_NUM = 2
} ranqi_msg_type_e;

typedef enum {
    SENSOR_PRESS = 0,
    SENSOR_TEMP  = 1,

    SENROR_NUM = 2
} sensor_type_e;


typedef enum {
    ENV_PRESS_OUT  = 0x0a,   // 辅助传感器，引出传感器
    ENV_PRESS_IN   = 0x1a,   // 主传感器，这设备传感器

    ENV_PRESS_NUM = 2
} env_pressure_e;

// #pragma pack(1)




typedef enum {
    MSG_RX_CHECK                    = 0x00,          // 接收检验错
    MSG_REPORT_RSSI                 = 0x02,          // 上报信号强度
    MSG_REPORT_BATTERY_Vol          = 0x03,          // 上传电池电压
    MSG_REPORT_DEVICE_Info          = 0x04,          // 上传设备信息
    MSG_REPORT_COM_Info             = 0x05,          // 上传器件信息
    MSG_REPORT_Sample_Inteval       = 0x06,          // 上传采集时间间隔
    MSG_REPORT_Report_Interval      = 0x07,          // 上传上传时间间隔
    MSG_REPORT_Report_Start_Limit   = 0x08,          // 上传上传起始限制
    MSG_REPORT_Report_Start_Kuang   = 0x09,          // 上传上传起始框定

    MSG_REPORT_Fang_bao_shui        = 0x11,          // 上传防爆防水信息
    MSG_REPORT_Start_lin            = 0x12,          // 上传初始倾斜角

    MSG_REPORT_Ncmd_Done            = 0x30,          // N型指令完成

    MSG_REPORT_Zero_Shift           = 0x40,          // 零漂
    MSG_REPORT_Xie_Shift            = 0x41,          // 斜漂

    MSG_REPORT_Crash_Acc_Limit      = 0x51,          // 撞击加速度门限值
#if PRODUCT_PORTS == 2 
    MSG_REPORT_Input_Over_Press_uplimit   = 0x52,          // 进口超压上限
    MSG_REPORT_Input_Over_Press_dnlimit   = 0x53,          // 进口超压下限
    MSG_REPORT_Input_Under_Press_uplimit  = 0x54,          // 进口欠压下限
    MSG_REPORT_Input_Under_Press_dnlimit  = 0x55,          // 进口欠压下限
    MSG_REPORT_Output_Over_Press_uplimit  = 0x56,          // 出口超压上限
    MSG_REPORT_Output_Over_Press_dnlimit  = 0x57,          // 出口超压下限
    MSG_REPORT_Output_Under_Press_uplimit = 0x58,          // 出口欠压上限
    MSG_REPORT_Output_Under_Press_dnlimit = 0x59,          // 出口欠压下限
    MSG_REPORT_Input_Temp_uplimit         = 0x5a,          // 进口温度上限
    MSG_REPORT_Input_Temp_dnlimit         = 0x5b,          // 进口温度下限
    MSG_REPORT_Output_Temp_uplimit        = 0x5e,          // 出口温度上限
    MSG_REPORT_Output_Temp_dnlimit        = 0x5f,          // 出口温度下限
#else 
    MSG_REPORT_Over_Press_uplimit         = 0x52,          // 超压上限
    MSG_REPORT_Over_Press_dnlimit         = 0x53,          // 超压下限
    MSG_REPORT_Under_Press_uplimit        = 0x54,          // 欠压下限
    MSG_REPORT_Under_Press_dnlimit        = 0x55,          // 欠压下限
    MSG_REPORT_Temp_uplimit               = 0x5a,          // 温度上限
    MSG_REPORT_Temp_dnlimit               = 0x5b,          // 温度下限    
#endif

    MSG_REPORT_Battery_low_limit          = 0x5c,          // 电池欠压门槛值
    MSG_REPORT_Tilt_limit                 = 0x5d,          // 倾斜门槛值
    
} msg_feedback_name_e;


typedef enum {
    ALM_Under_Press_Up      = 0x00,        // 欠压上限报警
    ALM_Under_Press_Dn      = 0x02,        // 欠压下限报警
    ALM_Over_Press_Dn       = 0xf0,        // 超压下限报警
    ALM_Over_Press_Up       = 0xf2,        // 超压上限报警
    ALM_Temp_Dn             = 0x01,        // 温度下限报警
    ALM_Temp_Up             = 0xf1,        // 温度上限报警
    ALM_Under_Press_Up_Clr  = 0x10,        // 欠压上限报警解除
    ALM_Under_Press_Dn_Clr  = 0x12,        // 欠压下限报警解除
    ALM_Over_Press_Dn_Clr   = 0xE0,        // 超压下限报警解除
    ALM_Over_Press_Up_Clr   = 0xE2,        // 超压上限报警解除
    ALM_Temp_Dn_Clr         = 0x11,        // 温度下限报警解除
    ALM_Temp_Un_Clr         = 0xE1,        // 温度上限报警解除
    ALM_Battery_Low         = 0x04,        // 电池欠压报警
    ALM_Tilt                = 0x05,        // 倾斜报警
    ALM_Crash_Acc           = 0x06,        // 撞击报警

	ALM_None                = 0xFF,
} ranqi_alarm_type_e;


typedef struct {
    uint8_t type;      // ranqi_alarm_type_e
    uint8_t loc;       // location  X0, X1, X2
    uint8_t value;     // value 可变长度
} __attribute__((packed))  alarm_info_s;


#if 1
typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   status;     // 0x00 表示出错
} __attribute__((packed)) rx_check_action_s;

typedef struct {
    uint8_t   name;      // 0x30
    uint8_t   size;
    uint8_t   data;      // 0x30
} __attribute__((packed)) ncmd_done_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint16_t  voltage;    // 保留2为小数， *100
} __attribute__((packed)) report_battry_voltage_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   rssi;
} __attribute__((packed)) report_rssi_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;       // 0x23
    char      hw_ver[2];   // 硬件版本号
    char      sw_ver[2];   // 软件版本号
    uint32_t  vendor_id;  // 0x00000006
    uint8_t   sn[4];      // BCD高字节在前  设备编号
    char      ICCID[20];  // 
    uint8_t   build_date[3];  // yyMMdd, BCD 高字节在前
} __attribute__((packed)) report_device_info_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;                // 0x25
    uint8_t   cpu_vendor;          // CPU厂家
    char      cpu_model[10];       // CPU型号
    uint8_t   nb_vendor;           // 通讯模块厂家
    char      nb_model[10];        // 通讯模块型号
    uint8_t   battery_vendor;       // 电池厂家
    char      battery_model[10];    // 电池型号
    float     battery_vol;          // 电池容量
} __attribute__((packed)) report_com_info_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;  // 0x1C
    char      fangbao_sn[12];       // 防爆证书编号
    char      fangbao_level[12];    // 防爆等级
    char      fangshui_level[4];    // 防水等级
} __attribute__((packed)) report_fang_bao_shui_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   interval;    
} __attribute__((packed)) report_sample_interval_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   interval;    
} __attribute__((packed)) report_report_interval_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   val;              // BCD format
} __attribute__((packed)) report_start_limit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   val;              // BCD format
} __attribute__((packed)) report_start_kuang_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;   // 0x18
    float     low_press_press;   // 3位小数点 
    float     low_press_temp;    // 1位小数点
    float     mid_press_press;   // 3位小数点 
    float     mid_press_temp;    // 1位小数点
    float     high_press_press;   // 3位小数点 
    float     high_press_temp;    // 1位小数点    
} __attribute__((packed)) report_zero_shift_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;   // 0x18
    float     low_press_press;   // 2.3位小数点 
    float     low_press_temp;    // 2.3位小数点
    float     mid_press_press;   // 2.3位小数点 
    float     mid_press_temp;    // 2.3位小数点 
    float     high_press_press;   // 2.3位小数点 
    float     high_press_temp;    // 2.3位小数点 
} __attribute__((packed)) report_xie_shift_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;  // 0x06
    int16_t   x;     // mg
    int16_t   y;     // mg
    int16_t   z;     // mg
} __attribute__((packed)) report_init_tilt_angle_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 2.3位小数点 
} __attribute__((packed)) report_crash_acc_limit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 单位KPa
} __attribute__((packed)) report_over_press_uplimit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 单位KPa
} __attribute__((packed)) report_over_press_dnlimit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 单位KPa
} __attribute__((packed)) report_under_press_uplimit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 单位KPa
} __attribute__((packed)) report_under_press_dnlimit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint16_t  limit;    // 0位小数点 , 0.01v
} __attribute__((packed)) report_battery_low_limit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 0.01v
} __attribute__((packed)) report_tilt_limit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;  // 0x0a
    float     lat;   
    char      n_s;
    float     lon;
    char      e_w;   
} __attribute__((packed)) report_loc_info_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 摄氏度
} __attribute__((packed)) report_temp_uplimit_s;

typedef struct {
    uint8_t   name; 
    uint8_t   size;
    float     limit;    // 0位小数点 , 摄氏度
} __attribute__((packed)) report_temp_dnlimit_s;

#endif


typedef struct {
    uint8_t   name; 
    uint8_t   size;
    uint8_t   val;    // 0x30
} __attribute__((packed)) report_ncmd_done_s;


/**************************************************/
/*                  Downstream message            */
/**************************************************/

typedef struct {
    uint8_t   head[2];   // 0x55 + 0xaa
    uint32_t  pkg_id;    // 0x80000000 ~ 0xFFFFFFFF
    uint8_t   type;      // 0x33
} __attribute__((packed)) ranqi_msg_dn_head_s;

typedef struct {
    uint32_t  ts;
    uint16_t  checksum;
    uint8_t   fofo;      // 0xFF
} __attribute__((packed)) ranqi_msg_dn_tail_s;

typedef struct {
    uint16_t  checksum;
    uint8_t   fofo;      // 0xFF
} __attribute__((packed)) ranqi_msg_dn_nots_tail_s;

typedef enum {
    ACK_TYPE_FAIL   = 0x00,
    ACK_TYPE_PASS   = 0x01,
} ack_type_e;

typedef enum{
    ALM_OBJ_UNDER_PRESS_UP_LIMIT     = 0x00,     // 欠压上限
    ALM_OBJ_UNDER_PRESS_DN_LIMIT     = 0x03,     // 欠压下限
    ALM_OBJ_OVER_PRESS_DN_LIMIT      = 0xf0,     // 超压下限  中压<=650kPa,  低压 <= 100kPa
    ALM_OBJ_OVER_PRESS_UP_LIMIT      = 0xf3,     // 超压上限
    ALM_OBJ_TEMP_DN_LIMIT            = 0x01,     // 温度下限  <= 60° 
    ALM_OBJ_TEMP_UP_LIMIT            = 0xf1,     // 温度上限  >= -20°
    ALM_OBJ_TILT_ANGLE               = 0x02,     // 倾斜角 <=65°， 两位正数的浮点型
    ALM_OBJ_CRASH_ACC                = 0x04,     // 撞击加速度门槛值
} alarm_obj_e;


typedef enum{
#if PRODUCT_PORTS == 2
    ALM_LOC_LOW_PRESS_OUT          = 0x00,     // 低压区出口
    ALM_LOC_MID_PRESS_OUT          = 0x01,     // 中压区出口
    ALM_LOC_HIGH_PRESS_OUT         = 0x02,     // 次高压区出口   
    ALM_LOC_LOW_PRESS_IN           = 0x10,     // 低压区入口
    ALM_LOC_MID_PRESS_IN           = 0x11,     // 中压区入口
    ALM_LOC_HIGH_PRESS_IN          = 0x12,     // 次高压区入口
#else 
    ALM_LOC_LOW_PRESS              = 0x00,     // 低压区出口
    ALM_LOC_MID_PRESS              = 0x01,     // 中压区出口
    ALM_LOC_HIGH_PRESS             = 0x02,     // 次高压区出口
#endif
    ALM_LOC_CRASH_ACC              = 0x06,     // 撞击
    ALM_LOC_TILT_ANGLE             = 0x03,     // 倾斜角
    
} alarm_loc_e;


typedef struct {
    uint8_t   alm_obj;   
    uint8_t   alm_loc;   
    float     limit;
} __attribute__((packed)) alarm_setting_s;

typedef enum{
    PARAM_NAME_Alarm_Tolerance     = 0x11,     //报警容忍度
    PARAM_NAME_Fliter_Coef         = 0x12,     //滤波器系数
    // 上传起始限制（秒），是为了避免下位机在某个时刻蜂拥上传做出的分批处理。如上传起始限制在13秒，意味着下位机只有在其时钟在13秒时，才能开始上传数据。报警、信息反馈、信息立即上传不受该限制
    PARAM_NAME_Report_Start_Limit  = 0x22,     //上传起始限制  秒 <= 59
    // 上传起始框定（分钟），也是为了避免下位机在某个时刻蜂拥上传做出的分批处理。不过当上传时间间隔不是小时的整数倍，这个指标是无效的
    PARAM_NAME_Report_Start_Kuang  = 0x23,     //上传起始框定  分 <= 59
} param_name_e;

typedef struct {
    uint8_t   name;   
    uint8_t   value;           // 10进制数值
} __attribute__((packed)) sys_param_set_s;


// 当采集时间间隔与上传时间间隔都为ff时，采集时间间隔为2小时，上传时间间隔为24小时，该状态定义为待机状态。待机状态下，报警关闭
// 上传时间间隔 是 采样时间间隔的整数倍，倍数<=12
typedef struct {
    uint8_t   sample_interval;   // 单位分钟 >= 1,  默认值5分钟
    uint8_t   report_interval;   // 单位分钟 >= 1， 默认值1小时， 60分钟
} __attribute__((packed)) set_interval_s;


typedef enum {
    SHIFT_ADJUST_TYPE_PRESS  = 0x00,
    SHIFT_ADJUST_TYPE_TEMP   = 0x01,
    SHIFT_ADJUST_TYPE_NUM
} shift_adjust_type_e;

typedef enum {
    SHIFT_ADJUST_COEF_ZERO  = 0x00,
    SHIFT_ADJUST_COEF_XIE   = 0x01,
    SHIFT_ADJUST_COEF_NUM
} shift_adjust_coef_e;

typedef struct {
    uint8_t   loc;   // 采样点 X0 X1 X2
    uint8_t   type;  // 类型
    uint8_t   coef;  // 系数项
    float     param; // 参数
} __attribute__((packed)) shift_adjust_s;

typedef struct {
    uint32_t   server_ip;
    uint16_t   server_port;
} __attribute__((packed)) server_info_s;

typedef enum{
    PARAM_TYPE_Battery_Vol         = 0x00,    // 电池电压
    PARAM_TYPE_Rssi                = 0x01,    // 信号强度
    PARAM_TYPE_Dev_Info            = 0x02,    // 设备信息
    PARAM_TYPE_Dev_Com_Info        = 0x03,    // 器件信息
    PARAM_TYPE_Fang_Bao_Shui       = 0x04,    // 防爆防水信息
    PARAM_TYPE_Sample_Interval     = 0x05,    // 采样时间间隔
    PARAM_TYPE_Report_Interval     = 0x06,    // 上传时间间隔
    PARAM_TYPE_Report_Start_Limit  = 0x07,    // 上传时间限制
    PARAM_TYPE_Report_Start_Kuang  = 0x08,    // 上传时间框定
    PARAM_TYPE_Zero_Shift          = 0x09,    // 零漂
    PARAM_TYPE_Xie_Shift           = 0x0a,    // 斜漂
    PARAM_TYPE_Init_Tilt_Angle     = 0x12,    // 初始倾斜角
    PARAM_TYPE_Crash_Acc_Limit     = 0x13,    // 上传撞击加速度门槛值


#if PRODUCT_PORTS == 2
    PARAM_TYPE_Input_Over_Press_Up_Limit           = 0x14,    // 上传进口超压上限
    PARAM_TYPE_Input_Over_Press_Dn_Limit           = 0x15,    // 上传进口超压下限
    PARAM_TYPE_Input_Under_Press_Up_Limit          = 0x16,    // 上传进口欠压上限
    PARAM_TYPE_Input_Under_Press_Dn_Limit          = 0x17,    // 上传进口欠压下限
    PARAM_TYPE_Output_Over_Press_Up_Limit          = 0x18,    // 上传出口超压上限
    PARAM_TYPE_Output_Over_Press_Dn_Limit          = 0x19,    // 上传出口超压下限
    PARAM_TYPE_Output_Under_Press_Up_Limit         = 0x1a,    // 上传出口欠压上限
    PARAM_TYPE_Output_Under_Press_Dn_Limit         = 0x1b,    // 上传出口欠压下限
#else 
    PARAM_TYPE_Over_Press_Up_Limit                 = 0x14,    // 上传超压上限
    PARAM_TYPE_Over_Press_Dn_Limit                 = 0x15,    // 上传超压下限
    PARAM_TYPE_Under_Press_Up_Limit                = 0x16,    // 上传欠压上限
    PARAM_TYPE_Under_Press_Dn_Limit                = 0x17,    // 上传欠压下限
#endif

    PARAM_TYPE_Battery_Low_Limit                   = 0x1c,    // 上传电池欠压门限值
    PARAM_TYPE_Tilt_Limit                          = 0x1f,    // 上传倾斜门限值
    PARAM_TYPE_Dev_Loc                             = 0x20,    // 地理位置信息

#if PRODUCT_PORTS == 2
    PARAM_TYPE_Input_Temp_Up_Limit                 = 0x1d,    // 上传进口温度上限        
    PARAM_TYPE_Input_Temp_Dn_Limit                 = 0x1e,    // 上传进口温度下限     
    PARAM_TYPE_Output_Temp_Up_Limit                = 0x21,    // 上传出口温度上限        
    PARAM_TYPE_Output_Temp_Dn_Limit                = 0x22,    // 上传出口温度下限
#else 
    PARAM_TYPE_Temp_Up_Limit                       = 0x1d,    // 上传温度上限        
    PARAM_TYPE_Temp_Dn_Limit                       = 0x1e,    // 上传温度下限     
#endif

} report_param_type_e;


/***********************************************/
/*              Build Message                  */
/***********************************************/
extern uint32_t build_up_normal_msg(uint8_t *buff, sensor_data_s *s_data);
extern uint32_t build_up_more_msg(uint8_t *buff);
extern uint32_t build_up_ack_msg(uint8_t *buff, uint8_t ack, uint32_t msg_id);
extern uint32_t build_up_feedback_msg(uint8_t *buff, uint32_t msg_id, uint8_t *act, uint32_t act_len);
extern uint32_t build_up_alarm_msg(uint8_t *buff, uint8_t *alm_info, uint32_t alm_len);
extern uint32_t build_up_manual_msg(uint8_t *buff);

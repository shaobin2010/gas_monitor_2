#include "include/at_device.h"
#include "ranqi_protocol.h"

extern uint32_t ranqi_dev_get_up_msg_id(void);

/***************************************************************/
/*                      Sensor Driver API                      */
/***************************************************************/
static float read_sensor_data(uint8_t port, uint8_t loc, sensor_type_e type)
{
    switch (loc) {
        case Ranqi_Sensor_Loc_Low:
            break;

        case Ranqi_Sensor_Loc_Mid:
            // TODO
            break;

        case Ranqi_Sensor_Loc_High:
            break;
        default:
            break;
    }
    return 1.25;
}

/***************************************************************/
/*                      Up Normal Message                      */
/***************************************************************/
// 发送后启动10s定时器，
//     如果没有收到上位机反馈，错误记录+1， 数据重发
//     如果收到检验错误反馈，错误记录+1， 数据重发
//     如果收到正确反馈，需要等待10s才能休眠
//     重发超过3次后，不再重发，并记录标记出来， 下个上报周期再补发 
//     重发消息ID不改变

typedef struct {
    uint8_t   head[2];   // 0xaa + 0x55
    uint32_t  msg_id;    // 0x00000000 ~ 0x7FFFFFFF
    uint8_t   type;      // 0x22
    uint8_t   imei[15];  // IMEI
    uint8_t   group_num; // 篇章总数m  <= 12
} __attribute__((packed)) ranqi_up_msg_head_s;

typedef struct {
    uint16_t  checksum;
    uint8_t   fofo;      // 0xFF
} __attribute__((packed)) ranqi_up_msg_tail_s;

typedef struct {
    uint8_t type;     // type
    uint8_t loc;      // location
    float   value;    // value
} __attribute__((packed))  ranqi_seg_s;

typedef struct {
    uint8_t       seg_num;
    ranqi_seg_s   seg1;        // Pressure
    ranqi_seg_s   seg2;        // Temp

#if PRODUCT_PORTS == 2    
    ranqi_seg_s   seg3;        // Environment Pressure
    ranqi_seg_s   seg4;        // Environment Pressure
#endif    

    uint32_t      ts;
} __attribute__((packed)) ranqi_group_s;

void build_seg(ranqi_seg_s *p_seg, uint8_t port, uint8_t loc, sensor_type_e type)
{
   p_seg->type   = type;
   p_seg->loc    = (uint8_t)((port << 4) | loc);
   p_seg->value  = read_sensor_data(port, loc, type);
}

void build_groups(uint8_t *buff)
{
    ranqi_group_s *p_grp = (ranqi_group_s *)buff;

    // group 1
    p_grp->seg_num = 0x02; 
    build_seg(&p_grp->seg1, Ranqi_Port_0, Ranqi_Sensor_Loc_Low, SENSOR_PRESS);
    build_seg(&p_grp->seg2, Ranqi_Port_0, Ranqi_Sensor_Loc_Low, SENSOR_TEMP);

#if PRODUCT_PORTS == 2
    p_grp->seg_num = 0x04; 
    build_seg(&p_grp->seg3, Ranqi_Port_1, Ranqi_Sensor_Loc_Low, SENSOR_PRESS);
    build_seg(&p_grp->seg4, Ranqi_Port_1, Ranqi_Sensor_Loc_Low, SENSOR_TEMP);
#endif    
    p_grp->ts = DS1302_Read_Unix_Time();
    p_grp++;

    // group 2
    p_grp->seg_num = 0x02; 
    build_seg(&p_grp->seg1, Ranqi_Port_0, Ranqi_Sensor_Loc_Mid, SENSOR_PRESS);
    build_seg(&p_grp->seg2, Ranqi_Port_0, Ranqi_Sensor_Loc_Mid, SENSOR_TEMP);

#if PRODUCT_PORTS == 2
    p_grp->seg_num = 0x04; 
    build_seg(&p_grp->seg3, Ranqi_Port_1, Ranqi_Sensor_Loc_Mid, SENSOR_PRESS);
    build_seg(&p_grp->seg4, Ranqi_Port_1, Ranqi_Sensor_Loc_Mid, SENSOR_TEMP);
#endif    
    p_grp->ts = DS1302_Read_Unix_Time();
    p_grp++;

    // group 3
    p_grp->seg_num = 0x02; 
    build_seg(&p_grp->seg1, Ranqi_Port_0, Ranqi_Sensor_Loc_High, SENSOR_PRESS);
    build_seg(&p_grp->seg2, Ranqi_Port_0, Ranqi_Sensor_Loc_High, SENSOR_TEMP);

#if PRODUCT_PORTS == 2
    p_grp->seg_num = 0x04; 
    build_seg(&p_grp->seg3, Ranqi_Port_1, Ranqi_Sensor_Loc_High, SENSOR_PRESS);
    build_seg(&p_grp->seg4, Ranqi_Port_1, Ranqi_Sensor_Loc_High, SENSOR_TEMP);
#endif   
    p_grp->ts = DS1302_Read_Unix_Time();
}

uint32_t build_up_normal_msg(uint8_t *buff)
{
    uint32_t len = 0;

    ranqi_up_msg_head_s *msg_head = (ranqi_up_msg_head_s *)buff;

    msg_head->head[0] = 0xaa;
    msg_head->head[1] = 0x55;
    msg_head->msg_id  = ranqi_dev_get_up_msg_id();
    msg_head->type    = MSG_NORNAL;
    memcpy(msg_head->imei, atDevice_get_imei(), IMEI_LEN);

    msg_head->group_num = 3; // 3 groups
    len = sizeof(ranqi_up_msg_head_s);

    build_groups(buff + len);   // TODO RC4 加密 
    len += msg_head->group_num*sizeof(ranqi_group_s);

    ranqi_up_msg_tail_s *msg_tail = (ranqi_up_msg_tail_s *)(buff +len);

    msg_tail->checksum = utils_checksum(buff, len);
    msg_tail->fofo = 0xFF;

    return len + sizeof(ranqi_up_msg_tail_s);
}



/***************************************************************/
/*                      补发数据 Message                      */
/***************************************************************/
// 发送后启动10s定时器，
//     如果没有收到上位机反馈，错误记录+1， 数据重发
//     如果收到检验错误反馈，错误记录+1， 数据重发
//     如果收到正确反馈，需要等待10s才能休眠
//     重发超过3次后，不再重发，并记录标记出来， 下个上报周期再补发 
//     重发消息ID不改变
//     上传间隔小于5分钟时，数据不补发
//     补发数据每次不超过20条

typedef struct {
    uint8_t   head[2];   // 0xaa + 0x55
    uint32_t  msg_id;    // 0x00000000 ~ 0x7FFFFFFF
    uint8_t   type;      // 0xCC
    uint8_t   imei[15];  // IMEI
} __attribute__((packed)) ranqi_up_more_head_s;


uint32_t build_up_more_msg(uint8_t *buff)
{
    uint32_t len = 0;

    ranqi_up_more_head_s *msg_head = (ranqi_up_more_head_s *)buff;
    msg_head->head[0] = 0xaa;
    msg_head->head[1] = 0x55;
    msg_head->msg_id  = ranqi_dev_get_up_msg_id();
    msg_head->type    = MSG_MORE;
    memcpy(msg_head->imei, ranqi_get_imei(), IMEI_LEN);
    len = sizeof(ranqi_up_more_head_s);

    // Build one group
    ranqi_group_s *p_grp = (ranqi_group_s *)(buff + len);
    p_grp->seg_num = 0x02; 
    build_seg(&p_grp->seg1, Ranqi_Port_0, Ranqi_Sensor_Loc_Low, SENSOR_PRESS);
    build_seg(&p_grp->seg2, Ranqi_Port_0, Ranqi_Sensor_Loc_Low, SENSOR_TEMP);
#if PRODUCT_PORTS == 2
    p_grp->seg_num = 0x04; 
    build_seg(&p_grp->seg3, Ranqi_Port_1, Ranqi_Sensor_Loc_Low, SENSOR_PRESS);
    build_seg(&p_grp->seg4, Ranqi_Port_1, Ranqi_Sensor_Loc_Low, SENSOR_TEMP);
#endif    
    p_grp->ts = DS1302_Read_Unix_Time();
    len += sizeof(ranqi_group_s);

    ranqi_up_msg_tail_s *msg_tail = (ranqi_up_msg_tail_s *)(buff +len);

    msg_tail->checksum = utils_checksum(buff, len);
    msg_tail->fofo = 0xFF;

    return len + sizeof(ranqi_up_msg_tail_s);
}

/***************************************************************/
/*                      信息反馈消息 Message                   */
/***************************************************************/
// N型指令，是指不需要回馈具体数据的指令：授时、设置报警上下限、设置滤波系数、设置报警容忍度、设置倾斜初始角，

// 下位机接到指令（非回馈消息），
//     1. 必须先校验，将校验结果正确与否回馈，
//     2. 然后执行指令，执行完成，需要上传数据的直接上传数据，
//     3. 不需要上传具体数据的指令发送指令完成信息
//     发送完成，启动10秒定时器，没有其它指令，转入休眠。

typedef struct {
    uint8_t   head[2];   // 0xaa + 0x55
    uint32_t  pkg_id;    // 0x00000000 ~ 0x7FFFFFFF
    uint8_t   type;      // 0x33
    uint8_t   imei[15];  // IMEI
} __attribute__((packed)) ranqi_up_feedback_head_s;

uint32_t build_up_feedback_msg(uint8_t *buff, uint32_t msg_id, uint8_t *act, uint32_t act_len)
{
    uint32_t len = 0;
    ranqi_up_msg_tail_s *msg_tail;
    ranqi_up_feedback_head_s *msg_head = (ranqi_up_feedback_head_s *)buff;

    msg_head->head[0] = 0xaa;
    msg_head->head[1] = 0x55;
    msg_head->pkg_id  = msg_id;
    msg_head->type    = MSG_FEEDBACK;
    memcpy(msg_head->imei, atDevice_get_imei(), IMEI_LEN);
    len = sizeof(ranqi_up_feedback_head_s);

    memcpy(&buff[len], act, act_len);
    len = len + act_len;

    msg_tail = (ranqi_up_msg_tail_s *)(buff + len);
    msg_tail->checksum = utils_checksum(buff, len);
    msg_tail->fofo = 0xFF;

    return len + sizeof(ranqi_up_msg_tail_s);
}


/***************************************************************/
/*                      报警消息 Message                      */
/***************************************************************/
// 数据：一个篇章报警时刻的即时数据（不含篇章数，数据格式与补发一致，数据不加密）
// 倾斜报警，采样点：0x03  数据格式为：X轴（2byte）+Y轴（2byte）+Z轴（2byte）。
//           在上位机下发“设置倾斜初始角”指令前，不进行倾斜与撞击报警
// 报警策略：
//     出现警讯，马上上传报警，等10秒，如果上位机没有新的采集上报策略，但有回馈,休眠。
//        后续检测值没有进入过正常范围，不重复报警。 
//        如果没有回馈，重发两次，每次重发之后等10秒，
//        两次重发仍然没有回馈，数据及报警存入补发数据区，待下次通讯完成，优先补发
// 当欠压下限为0时，低于0才报警；等于0不报警
// 电池欠压报警，采样点填0x03。电池电压值：单位0.01V，16进制，两个字节
// 撞击报警，采样点填0x06。

typedef struct {
    uint8_t   head[2];   // 0xaa + 0x55
    uint32_t  pkg_id;    // 0x00000000 ~ 0x7FFFFFFF
    uint8_t   type;      // 0x11
    uint8_t   imei[15];  // IMEI
} __attribute__((packed)) ranqi_up_alarm_head_s;

typedef struct {
    uint32_t  ts;
    uint16_t  checksum;
    uint8_t   fofo;      // 0xFF
} __attribute__((packed)) ranqi_up_msg_ts_tail_s;

uint32_t build_up_alarm_msg(uint8_t *buff, uint8_t *alm_info, uint32_t alm_len)
{
    uint32_t len = 0;
    ranqi_up_msg_ts_tail_s *msg_tail;
    ranqi_up_alarm_head_s *msg_head = (ranqi_up_alarm_head_s *)buff;

    msg_head->head[0] = 0xaa;
    msg_head->head[1] = 0x55;
    msg_head->pkg_id  = ranqi_dev_get_up_msg_id();
    msg_head->type    = MSG_ALARM;
    memcpy(msg_head->imei, ranqi_get_imei(), IMEI_LEN);
    len = sizeof(ranqi_up_feedback_head_s);

    memcpy(&buff[len], alm_info, alm_len);
    len = len + alm_len;

    msg_tail = (ranqi_up_msg_ts_tail_s *)(buff + len);
    msg_tail->ts = DS1302_Read_Unix_Time();
    msg_tail->checksum = utils_checksum(buff, len + 4);
    msg_tail->fofo = 0xFF;

    return len + sizeof(ranqi_up_msg_ts_tail_s);
}

/***************************************************************/
/*                      手动上传 Message                      */
/***************************************************************/
// 软件版本号去掉中间小数点，用2个字节，传ASCII码
// 手动上传一旦启动，待机状态（定义见4.4）自动解除

typedef struct {
    uint8_t   head[2];   // 0xaa + 0x55
    uint32_t  pkg_id;    // 0x00000000 ~ 0x7FFFFFFF
    uint8_t   type;      // 0x44
    uint8_t   imei[15];  // IMEI
    char      sw_ver[2]; // 软件版本号
} __attribute__((packed)) ranqi_up_manual_head_s;


uint32_t build_up_manual_msg(uint8_t *buff)
{
    uint32_t len = 0;

    ranqi_up_manual_head_s *msg_head = (ranqi_up_manual_head_s *)buff;
    msg_head->head[0] = 0xaa;
    msg_head->head[1] = 0x55;
    msg_head->pkg_id  = ranqi_dev_get_up_msg_id();
    msg_head->type    = MSG_MUNUAL;
    memcpy(msg_head->imei, ranqi_get_imei(), IMEI_LEN);
    memcpy(msg_head->sw_ver, Ranqi_Dev_Sw_Ver, 2);
    len = sizeof(ranqi_up_manual_head_s);

    // Build one group
    ranqi_group_s *p_grp = (ranqi_group_s *)(buff + len);
    p_grp->seg_num = 0x02; 
    build_seg(&p_grp->seg1, Ranqi_Port_0, Ranqi_Sensor_Loc_Low, SENSOR_PRESS);
    build_seg(&p_grp->seg2, Ranqi_Port_0, Ranqi_Sensor_Loc_Low, SENSOR_TEMP);
#if PRODUCT_PORTS == 2
    p_grp->seg_num = 0x04; 
    build_seg(&p_grp->seg3, Ranqi_Port_1, Ranqi_Sensor_Loc_Low, SENSOR_PRESS);
    build_seg(&p_grp->seg4, Ranqi_Port_1, Ranqi_Sensor_Loc_Low, SENSOR_TEMP);
#endif    
    p_grp->ts = DS1302_Read_Unix_Time();
    len += sizeof(ranqi_group_s);

    ranqi_up_msg_tail_s *msg_tail = (ranqi_up_msg_tail_s *)(buff +len);

    msg_tail->checksum = utils_checksum(buff, len);
    msg_tail->fofo = 0xFF;

    return len + sizeof(ranqi_up_msg_tail_s);
}

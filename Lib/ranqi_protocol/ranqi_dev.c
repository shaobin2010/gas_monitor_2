
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//  Driver header
#include "include/at_device.h"

//  protocal and utils header
#include "board.h"
#include "ranqi_dev.h"
#include "ranqi_protocol.h"
#include "utils.h"

#include "main.h"

#define TX_FRAME_BUF_LEN   15

static ranqi_device_s ranqi_dev;
static uint8_t tx_frame_buffer[TX_FRAME_BUF_LEN];

/************************************************************************/
/*                     Send Normal Frame                               */
/************************************************************************/

uint32_t ranqi_dev_get_up_msg_id(void)
{
    uint32_t val = ranqi_dev.up_pkg_id;
    ranqi_dev.up_pkg_id++;
    if(ranqi_dev.up_pkg_id >= 0x80000000) {  // 0x00000000 ~ 0x7FFFFFFF
        ranqi_dev.up_pkg_id = 0;
    }

    return val;
}

/************************************************************************/
/*                     Send Resonse Frame                               */
/************************************************************************/
static void send_response_ack(ack_type_e ack, uint32_t msg_id)
{
    uint32_t msg_len;
    rx_check_action_s action;

    action.name = MSG_RX_CHECK;
    action.size = 0x01;
    action.status = ack;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void send_NCMD_done(uint32_t msg_id)
{
    uint32_t msg_len;
    ncmd_done_s action;

    action.name = MSG_REPORT_Ncmd_Done;
    action.size = 1;
    action.data = 0x30;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Battery_Voltage(uint32_t msg_id)
{
    uint32_t msg_len;
    report_battry_voltage_s action;

    action.name = MSG_REPORT_BATTERY_Vol; 
    action.size = 0x02;
    action.voltage = 375; // 由于没有电池，所以hardware为3.75v

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Rssi(uint32_t msg_id)
{
    uint32_t msg_len;
    report_rssi_s action;

    action.name = MSG_REPORT_RSSI; 
    action.size = 0x01;
    module_get_rssi(&action.rssi);

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Device_info(uint32_t msg_id)
{
    uint32_t msg_len;
    report_device_info_s action;

    action.name = MSG_REPORT_DEVICE_Info; 
    action.size = sizeof(report_device_info_s) - 2;
    memcpy(action.hw_ver, Ranqi_Dev_Hw_Ver, 2);
    memcpy(action.sw_ver, Ranqi_Dev_Sw_Ver, 2);
    action.vendor_id = Ranqi_Dev_Vendor_Id;
    memcpy(action.ICCID, atDevice_get_iccid, ICCID_LEN);
    action.sn[0] = (uint8_t)(Ranqi_Dev_SN >> 24);
    action.sn[1] = (uint8_t)(Ranqi_Dev_SN >> 16);
    action.sn[2] = (uint8_t)(Ranqi_Dev_SN >> 8);
    action.sn[3] = (uint8_t)Ranqi_Dev_SN;

    action.build_date[0] = (uint8_t)(Rqnqi_Dev_Date >> 16);
    action.build_date[1] = (uint8_t)(Rqnqi_Dev_Date >> 8);
    action.build_date[2] = (uint8_t)Rqnqi_Dev_Date;    

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Device_Com_info(uint32_t msg_id)
{
    uint32_t msg_len;
    report_com_info_s action;

    action.name = MSG_REPORT_COM_Info; 
    action.size = sizeof(report_com_info_s) - 2;
    action.cpu_vendor = Ranqi_CPU_Vendor;
    memcpy(action.cpu_model, Ranqi_CPU_Model, strlen(Ranqi_CPU_Model));
    action.nb_vendor = Ranqi_NB_Vendor;
    memcpy(action.nb_model, Ranqi_NB_Model, strlen(Ranqi_NB_Model));
    action.battery_vendor = Ranqi_BT_Vendor;
    memcpy(action.battery_model, Ranqi_BT_Model, strlen(Ranqi_BT_Model));
    action.battery_vol = (float)Ranqi_BT_Vol;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Fang_Bao_Shui(uint32_t msg_id)
{
    uint32_t msg_len;
    report_fang_bao_shui_s action;

    action.name = MSG_REPORT_Fang_bao_shui; 
    action.size = sizeof(report_fang_bao_shui_s) - 2;

    memcpy(action.fangbao_sn, Fang_Bao_Cert_SN, strlen(Fang_Bao_Cert_SN));
    memcpy(action.fangbao_level, Fang_Bao_Level, strlen(Fang_Bao_Level));
    memcpy(action.fangshui_level, Fang_Shui_Level, strlen(Fang_Shui_Level));

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Sample_Interval(uint32_t msg_id)
{
    uint32_t msg_len;
    report_sample_interval_s action;

    action.name = MSG_REPORT_Sample_Inteval; 
    action.size = 1;
    action.interval = ranqi_dev.sample_interval;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Report_Interval(uint32_t msg_id)
{
    uint32_t msg_len;
    report_report_interval_s action;

    action.name = MSG_REPORT_Report_Interval; 
    action.size = 1;
    action.interval = ranqi_dev.report_interval;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Start_Limit(uint32_t msg_id)
{
    uint32_t msg_len;
    report_start_limit_s action;

    action.name = MSG_REPORT_Report_Start_Limit; 
    action.size = 1;
    action.val = ranqi_dev.report_start_limit/10*16 + ranqi_dev.report_start_limit%10;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Start_Kuang(uint32_t msg_id)
{
    uint32_t msg_len;
    report_start_kuang_s action;

    action.name = MSG_REPORT_Report_Start_Kuang; 
    action.size = 1;
    action.val = ranqi_dev.report_start_kuang/10*16 + ranqi_dev.report_start_kuang%10;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Zero_Shift(uint32_t msg_id)
{
    uint32_t msg_len;
    report_zero_shift_s action;

    action.name = MSG_REPORT_Zero_Shift; 
    action.size = sizeof(report_zero_shift_s) - 2;
    action.low_press_press  = 1.111;        // TODO....
    action.low_press_temp   = 24.1;         // TODO....
    action.mid_press_press  = 1.111;        // TODO....
    action.mid_press_temp   = 24.1;         // TODO....
    action.high_press_press = 1.111;        // TODO....
    action.high_press_temp  = 24.1;         // TODO....

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Xie_Shift(uint32_t msg_id)
{
    uint32_t msg_len;
    report_xie_shift_s action;

    action.name = MSG_REPORT_Xie_Shift; 
    action.size = sizeof(report_xie_shift_s) - 2;
    action.low_press_press  = 1.111;        // TODO....
    action.low_press_temp   = 24.1;         // TODO....
    action.mid_press_press  = 1.111;        // TODO....
    action.mid_press_temp   = 24.1;         // TODO....
    action.high_press_press = 1.111;        // TODO....
    action.high_press_temp  = 24.1;         // TODO....

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Init_Tilt_Angle(uint32_t msg_id)
{
    uint32_t msg_len;
    report_init_tilt_angle_s action;

    action.name = MSG_REPORT_Xie_Shift; 
    action.size = sizeof(report_init_tilt_angle_s) - 2;
    action.x = ranqi_dev.init_tilt_angle.x;  
    action.y = ranqi_dev.init_tilt_angle.y;
    action.z = ranqi_dev.init_tilt_angle.z;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Crash_Acc_Limit(uint32_t msg_id)
{
    uint32_t msg_len;
    report_crash_acc_limit_s action;

    action.name = MSG_REPORT_Crash_Acc_Limit; 
    action.size = 4;
    action.limit = ranqi_dev.crash_acc_limit;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Battery_Low_Limit(uint32_t msg_id)
{
    uint32_t msg_len;
    report_battery_low_limit_s action;

    action.name = MSG_REPORT_Battery_low_limit; 
    action.size = 4;
    action.limit = ranqi_dev.battry_low_limit;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Tilt_Limit(uint32_t msg_id)
{
    uint32_t msg_len;
    report_tilt_limit_s action;

    action.name = MSG_REPORT_Tilt_limit; 
    action.size = 4;
    action.limit = ranqi_dev.tilt_limit;

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}


static void Report_Device_Location(uint32_t msg_id)
{
    uint32_t msg_len;
    report_loc_info_s action;

    memset(&action, 0, sizeof(action));
    action.name = MSG_REPORT_Tilt_limit; 
    action.size = sizeof(report_loc_info_s) - 2;

#if 0
    if(atgm332d_get_latitude(&action.lat))
    {
        atgm332d_get_longitude(&action.lon);
        atgm332d_get_n_s(&action.n_s);
        atgm332d_get_e_w(&action.e_w);
    } else {
        printf("Could not get GPS data \r\n");
    }
#endif

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}


#if PRODUCT_PORTS == 2

static void Report_Over_Press_Up_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_over_press_uplimit_s action;

    action.name = port ? MSG_REPORT_Input_Over_Press_uplimit : MSG_REPORT_Output_Over_Press_uplimit; 
    action.size = 4;
    action.limit = ranqi_dev.over_press_up_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Over_Press_Dn_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_over_press_dnlimit_s action;

    action.name = port ? MSG_REPORT_Input_Over_Press_dnlimit : MSG_REPORT_Output_Over_Press_dnlimit;
    action.size = 4;
    action.limit = ranqi_dev.over_press_dn_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Under_Press_Up_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_under_press_uplimit_s action;

    action.name = port ? MSG_REPORT_Input_Under_Press_uplimit : MSG_REPORT_Output_Under_Press_uplimit; 
    action.size = 4;
    action.limit = ranqi_dev.under_press_up_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Under_Press_Dn_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_under_press_dnlimit_s action;

    action.name = port ? MSG_REPORT_Input_Under_Press_dnlimit : MSG_REPORT_Output_Under_Press_dnlimit; 
    action.size = 4;
    action.limit = ranqi_dev.under_press_dn_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Temp_Up_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_temp_uplimit_s action;

    idx = get_alm_loc_idx(ALM_LOC_LOW_PRESS_IN);

    action.name = port ? MSG_REPORT_Input_Temp_uplimit : MSG_REPORT_Output_Temp_uplimit; 
    action.size = 4;
    action.limit = ranqi_dev.temp_up_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Temp_Dn_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_temp_dnlimit_s action;

    action.name = port ? MSG_REPORT_Input_Temp_dnlimit : MSG_REPORT_Output_Temp_dnlimit; 
    action.size = 4;
    action.limit = ranqi_dev.temp_dn_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

#else

static void Report_Over_Press_Up_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_over_press_uplimit_s action;

    action.name = MSG_REPORT_Over_Press_uplimit; 
    action.size = 4;
    action.limit = ranqi_dev.over_press_up_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Over_Press_Dn_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_over_press_dnlimit_s action;

    action.name = MSG_REPORT_Over_Press_dnlimit; 
    action.size = 4;
    action.limit = ranqi_dev.over_press_dn_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Under_Press_Up_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_under_press_uplimit_s action;

    action.name = MSG_REPORT_Under_Press_uplimit; 
    action.size = 4;
    action.limit = ranqi_dev.under_press_up_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Under_Press_Dn_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_under_press_dnlimit_s action;

    action.name = MSG_REPORT_Under_Press_dnlimit; 
    action.size = 4;
    action.limit = ranqi_dev.under_press_dn_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Temp_Up_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_temp_uplimit_s action;

    action.name = MSG_REPORT_Temp_uplimit; 
    action.size = 4;
    action.limit = ranqi_dev.temp_up_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}

static void Report_Temp_Dn_Limit(uint32_t msg_id, uint8_t port, uint8_t pos)
{
    uint32_t msg_len;
    report_temp_dnlimit_s action;

    
    action.name = MSG_REPORT_Temp_dnlimit; 
    action.size = 4;
    action.limit = ranqi_dev.temp_dn_limit[pos][port];

    msg_len = build_up_feedback_msg(tx_frame_buffer, msg_id, (uint8_t *)&action, sizeof(action));
    ctwing_send_data(0, tx_frame_buffer, msg_len);
}
#endif


/************************************************************************/
/*                           RX Frame Process                           */
/************************************************************************/
static bool validate_frame(uint8_t *buff, uint16_t len)
{
    uint16_t head_body_len;
    ranqi_msg_dn_tail_s *tail;
    ranqi_msg_dn_nots_tail_s *nots_tail;

    ranqi_msg_dn_head_s *head = (ranqi_msg_dn_head_s *)buff;
    if ((head->head[0] != 0x55) || (head->head[0] != 0xaa)) {
        return false;
    }

    if ((head->pkg_id < 0x80000000)) {
        return false;
    }

    if ( (head->type == MSG_INIT_TILT_ANGLE) || 
         (head->type == MSG_SLEEP) ) {
        head_body_len = sizeof(ranqi_msg_dn_head_s); 
        nots_tail = (ranqi_msg_dn_nots_tail_s *)buff[head_body_len];
        
        if(nots_tail->checksum != utils_checksum(buff, head_body_len)) {
            return false;
        }
    } else {
        head_body_len = len - sizeof(ranqi_msg_dn_tail_s);
        tail = (ranqi_msg_dn_tail_s *)buff[head_body_len];
        if(tail->checksum != utils_checksum(buff, head_body_len + 4)) {
            return false;
        }
    }
    
    return true;
}


static bool dn_alaram_settings_handler(alarm_setting_s *settings)
{
    bool ret;
    uint8_t port, pos;

    ret = true;
    port = RANQI_SENSOR_PORT_GET(settings->alm_loc);
    pos   = RANQI_SENSOR_POS_GET(settings->alm_loc);
    switch (settings->alm_obj){
        case ALM_OBJ_UNDER_PRESS_UP_LIMIT:
            if ( (port > Ranqi_Port_Num) || 
                 (pos > Ranqi_Sensor_Loc_Num) ) {
                return false;
            }

            ranqi_dev.under_press_up_limit[pos][port] = settings->limit;
            break;

        case ALM_OBJ_UNDER_PRESS_DN_LIMIT:
            if ( (port > Ranqi_Port_Num) || 
                 (pos > Ranqi_Sensor_Loc_Num) ) {
                return false;
            }
            ranqi_dev.under_press_dn_limit[pos][port] = settings->limit;
            break;

        case ALM_OBJ_OVER_PRESS_DN_LIMIT:
            if ( (port > Ranqi_Port_Num) || 
                 (pos > Ranqi_Sensor_Loc_Num) ) {
                return false;
            }
            ranqi_dev.over_press_dn_limit[pos][port] = settings->limit;
            break;

        case ALM_OBJ_OVER_PRESS_UP_LIMIT:
            if ( (port > Ranqi_Port_Num) || 
                 (pos > Ranqi_Sensor_Loc_Num) ) {
                return false;
            }
            ranqi_dev.over_press_up_limit[pos][port] = settings->limit;

            break;

        case ALM_OBJ_TEMP_DN_LIMIT:
            if ( (port > Ranqi_Port_Num) || 
                 (pos > Ranqi_Sensor_Loc_Num) ) {
                return false;
            }
            ranqi_dev.temp_dn_limit[pos][port] = settings->limit;

            break;

        case ALM_OBJ_TEMP_UP_LIMIT:
            if ( (port > Ranqi_Port_Num) || 
                 (pos > Ranqi_Sensor_Loc_Num) ) {
                return false;
            }
            ranqi_dev.temp_up_limit[pos][port] = settings->limit;

            break;

        case ALM_OBJ_TILT_ANGLE:
            if(settings->alm_loc == ALM_LOC_TILT_ANGLE) {
                ranqi_dev.tilt_limit= settings->limit;
            } else {
                ret = false;
            }
            break;

        case ALM_OBJ_CRASH_ACC:
            if(settings->alm_loc == ALM_LOC_CRASH_ACC) {
                ranqi_dev.crash_acc_limit= settings->limit;
            } else {
                ret = false;
            }
            break;

        default:
            ret = false;
            break;
    }

    return ret;
}


static bool dn_system_settings_handler(sys_param_set_s *settings)
{
    bool ret = true;
    switch (settings->name) {
        case PARAM_NAME_Alarm_Tolerance:
            ranqi_dev.alarm_tolerance = settings->value;
            break;

        case PARAM_NAME_Fliter_Coef:
            ranqi_dev.filter_coef = settings->value;
            break;

        case PARAM_NAME_Report_Start_Limit:
            ranqi_dev.report_start_limit = settings->value/16*10 + settings->value%16;
            break;

        case PARAM_NAME_Report_Start_Kuang:
            ranqi_dev.report_start_kuang = settings->value/16*10 + settings->value%16;
            break;

        default:
            ret = false;
            break;
    }

    return ret;
}

static bool dn_set_interval_handler(set_interval_s *settings)
{
    if ( (settings->sample_interval == 0) || 
         (settings->report_interval == 0) ||
         (settings->report_interval/settings->sample_interval > 12) ||
         (settings->report_interval%settings->sample_interval != 0)) {
        return false;
    }

    ranqi_dev.sample_interval = settings->sample_interval;
    ranqi_dev.report_interval = settings->report_interval;
    return true;
}

static bool dn_set_time_handler(uint32_t ts)
{
    DS1302_Write_Unix_Time(ts);
    return true;
}


static bool dn_request_report_handler(report_param_type_e type, uint32_t msg_id)
{
    bool ret = true;
    switch (type) {
        case PARAM_TYPE_Battery_Vol:
            Report_Battery_Voltage(msg_id);
            break;

        case PARAM_TYPE_Rssi:
            Report_Rssi(msg_id);
            break;

        case PARAM_TYPE_Dev_Info:
            Report_Device_info(msg_id);
            break;

        case PARAM_TYPE_Dev_Com_Info:
            Report_Device_Com_info(msg_id);
            break;

        case PARAM_TYPE_Fang_Bao_Shui:
            Report_Fang_Bao_Shui(msg_id);
            break;

        case PARAM_TYPE_Sample_Interval:
            Report_Sample_Interval(msg_id);
            // 待机状态下，上传0xFF
            break;

        case PARAM_TYPE_Report_Interval:
            Report_Report_Interval(msg_id);
            // 待机状态下，上传0xFF
            break;

        case PARAM_TYPE_Report_Start_Limit:
            Report_Start_Limit(msg_id);
            break;

        case PARAM_TYPE_Report_Start_Kuang:
            Report_Start_Kuang(msg_id);
            break;

        case PARAM_TYPE_Zero_Shift:
            Report_Zero_Shift(msg_id);
            break;

        case PARAM_TYPE_Xie_Shift:
            Report_Xie_Shift(msg_id);
            break;

        case PARAM_TYPE_Init_Tilt_Angle:
            Report_Init_Tilt_Angle(msg_id);
            break;

        case PARAM_TYPE_Crash_Acc_Limit:
            Report_Crash_Acc_Limit(msg_id);
            break;
            
#if PRODUCT_PORTS == 2
        case PARAM_TYPE_Input_Over_Press_Up_Limit:
            // TOOD ...  check location
            Report_Over_Press_Up_Limit(msg_id, Ranqi_Port_1, Ranqi_Sensor_Loc_Low);
            break;
        case PARAM_TYPE_Output_Over_Press_Up_Limit:
            // TOOD ...  check location     
            Report_Over_Press_Up_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;

        case PARAM_TYPE_Input_Over_Press_Dn_Limit:
            // TOOD ...  check location
            Report_Over_Press_Dn_Limit(msg_id, Ranqi_Port_1, Ranqi_Sensor_Loc_Low);
            break;
        case PARAM_TYPE_Output_Over_Press_Dn_Limit:
            // TOOD ...  check location
            Report_Over_Press_Dn_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;

        case PARAM_TYPE_Input_Under_Press_Up_Limit:
            // TOOD ...  check location
            Report_Under_Press_Up_Limit(msg_id, Ranqi_Port_1, Ranqi_Sensor_Loc_Low);
            break;
        case PARAM_TYPE_Output_Under_Press_Up_Limit:
            // TOOD ...  check location
            Report_Under_Press_Up_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break

        case PARAM_TYPE_Input_Under_Press_Dn_Limit:
            // TOOD ...  check location
            Report_Under_Press_Dn_Limit(msg_id, Ranqi_Port_1, Ranqi_Sensor_Loc_Low);
            break;

        case PARAM_TYPE_Output_Under_Press_Dn_Limit:
            // TOOD ...  check location
            Report_Under_Press_Dn_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;
#else 
        case PARAM_TYPE_Over_Press_Up_Limit:
            Report_Over_Press_Up_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;

        case PARAM_TYPE_Over_Press_Dn_Limit:
            Report_Over_Press_Dn_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;

        case PARAM_TYPE_Under_Press_Up_Limit:
            Report_Under_Press_Up_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;

        case PARAM_TYPE_Under_Press_Dn_Limit:
            Report_Under_Press_Dn_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;
#endif
        case PARAM_TYPE_Battery_Low_Limit:
            Report_Battery_Low_Limit(msg_id);
            break;

        case PARAM_TYPE_Tilt_Limit:
            Report_Tilt_Limit(msg_id);
            break;

        case PARAM_TYPE_Dev_Loc:
            Report_Device_Location(msg_id);
            break;

#if PRODUCT_PORTS == 2
        case PARAM_TYPE_Input_Temp_Up_Limit:
            Report_Input_Temp_Up_Limit(msg_id, Ranqi_Port_1, Ranqi_Sensor_Loc_Low);
            break;      
        case PARAM_TYPE_Input_Temp_Dn_Limit:
            Report_Input_Temp_Dn_Limit(msg_id, Ranqi_Port_1, Ranqi_Sensor_Loc_Low);
            break; 
        case PARAM_TYPE_Output_Temp_Up_Limit:
            Report_Output_Temp_Up_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;    
        case PARAM_TYPE_Output_Temp_Dn_Limit:
            Report_Output_Temp_Dn_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;
#else 
        case PARAM_TYPE_Temp_Up_Limit:
            Report_Temp_Up_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break;      

        case PARAM_TYPE_Temp_Dn_Limit:
            Report_Temp_Dn_Limit(msg_id, Ranqi_Port_0, Ranqi_Sensor_Loc_Low);
            break; 
#endif
        default:
            break;
    }

    return ret;
}

static bool dn_init_tilt_angle_handler(void)
{
#if 0
    ranqi_dev.init_tilt_angle.x = L3G4200D_Get_X();
    ranqi_dev.init_tilt_angle.y = L3G4200D_Get_Y();
    ranqi_dev.init_tilt_angle.z = L3G4200D_Get_Z(); 
#endif
    return true;
}

static bool dn_shift_adjust_handler(shift_adjust_s *settings)
{
    uint8_t port, pos;
    
    port = RANQI_SENSOR_PORT_GET(settings->loc);
    pos   = RANQI_SENSOR_POS_GET(settings->loc);
    if ( (port > Ranqi_Port_Num) || 
         (pos > Ranqi_Sensor_Loc_Num) || 
         (settings->type > SHIFT_ADJUST_TYPE_NUM) ||
         (settings->coef > SHIFT_ADJUST_COEF_NUM) ) {
        return false;
    }

    if (settings->coef == SHIFT_ADJUST_COEF_ZERO) {
        ranqi_dev.zero_shift_adjust[pos][settings->type][port] = settings->param;
    } else {
        ranqi_dev.xie_shift_adjust[pos][settings->type][port]  = settings->param;
    }
    
    return true;
}

static bool dn_server_info_handler(server_info_s *settings)
{
    ranqi_dev.server_ip = settings->server_ip;
    ranqi_dev.server_port = settings->server_port;

    // TODO ..... Store Server info into Flash
    return true;
}

static bool dn_device_sleep(void)
{
    return true;
}

static void dn_frame_process(uint8_t *buff, uint16_t len)
{
    uint8_t *body;
    ranqi_msg_dn_head_s *head = (ranqi_msg_dn_head_s *)buff;

    if(!validate_frame(buff, len)) {
        printf("Frame is not correct\r\n");
        send_response_ack(ACK_TYPE_FAIL, head->pkg_id);
        return;
    }

    body = buff + sizeof(ranqi_msg_dn_head_s);
    switch (head->type) {
        case MSG_ACK:
        	// N型指令, 不需要回馈
            if (body[0] == ACK_TYPE_PASS) {
                // success
            } else {
                // TODO....
            }
            break;
        
        case MSG_ALM_PARAM_SET:
        	// N型指令, 不需要回馈
             dn_alaram_settings_handler((alarm_setting_s *)body);
             break;

        case MSG_SYS_PARAM_SET:
        	// N型指令, 不需要回馈
            dn_system_settings_handler((sys_param_set_s *)body);
            break;

        case MSG_INTERVAL:
        	// N型指令, 不需要回馈
            dn_set_interval_handler((set_interval_s *)body);
            break;

        case MSG_TIME:
        	// N型指令, 不需要回馈
            dn_set_time_handler(((ranqi_msg_dn_tail_s *)body)->ts);
            break;
        
        case MSG_REQUEST_REPORT:
        	// 非N型指令, 需要回馈
        	send_response_ack(ACK_TYPE_PASS, head->pkg_id); // 反馈检验OK
            dn_request_report_handler(buff[0], head->pkg_id);
            break;

        case MSG_SHIFT_ADJUST:
        	// N型指令, 不需要回馈
            dn_shift_adjust_handler((shift_adjust_s *)body);
            break;

        case MSG_INIT_TILT_ANGLE:
        	// N型指令, 不需要回馈
            dn_init_tilt_angle_handler();
            break;

        case MSG_SERVER_INFO:
            dn_server_info_handler((server_info_s *)body);
            break;

        case MSG_SLEEP:
        	// N型指令, 不需要回馈消息
            dn_device_sleep();
            // TODO ... 是否需要N型指令完成报告
            break;

        default:
            break;
    }
}

void ranqi_dev_init(void)
{
    memset(&ranqi_dev, 0, sizeof(ranqi_dev));
    ranqi_dev.device_port = PRODUCT_PORTS;
    ranqi_dev.server_ip = Ranqi_Default_Server_Ip;
    ranqi_dev.server_port = Ranqi_Default_Server_Port;
    ranqi_dev.sample_interval = DEFAULT_SAMPLE_INTERVAL;
    ranqi_dev.report_interval = DEFAULT_REPORT_INTERVAL;
}

#include "include/at_device.h"
#include "include/at_client.h"
#include "include/at_utils.h"

extern void ranqi_tick_test(void);

QueueHandle_t Ranqi_Rx_Queue;
static Ranqi_xMessage_s ranqi_rx_msg;

void Ranqi_Tx_Task( void *pvParameters )
{
	ranqi_dev_init();
	if(at_device_init() != AT_SYS_RET_SUCCESS) {
		printf("Ranqi_Task failed, Exit Task");
		return;
	}

	atDevice_wait_network();
    while(1)
    {
    	atDevice_ctwing_connect(ranqi_dev.server_ip, ranqi_dev.server_port);


        printf( "Ranqi_Tx_Task output ........\r\n" );
        /* 延迟，以产生一个周期 */
        osDelay(2000);
    }
}

void Ranqi_Rx_Task( void *pvParameters )
{
    while(1)
    {
    	if(xQueueReceive(Ranqi_Rx_Queue, (void *)&ranqi_rx_msg, 3000*portTICK_PERIOD_MS) == pdFALSE) {
    		printf( "No Message from Server\r\n" );
    		continue;
    	}

		dn_frame_process(ranqi_rx_msg.ucData, ranqi_rx_msg.ucData_len);
    }
}


#if 0
void Ranqi_Task(void const * argument)
{
    at_device_init();
    ranqi_dev_init();

    while(1) {
        if(!check_at_network()) {
            osDelay(3000);
            continue;
        } else {
            break;
        }
    }
    
    printf("Ranqi_Task starting...\r\n");
    regester_ctwing_server();

    for(;;) {


        atgm332d_parse_gps_buffer();
        ranqi_tick_test();
        osDelay(1000);
    }
}

#endif

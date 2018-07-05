/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    Hm_bluetooth.h         
* 功能描述:    
       蓝牙模块串口通信协议头文件定义
* 版本      作者       时间          状态
* V1.0      sunlf      2015.10.14    创建文件
*****************************************************************************
*****************************************************************************/ 

#ifndef __HM_BLE_H
#define __HM_BLE_H




#define TEST_BLE              "AT"          //test mode
#define QUES_BLE_MAC          "AT+ADDB?"
#define SET_EDR_DEVICE_NAME   "AT+NAME"    //"AT+NAMEE[para1]"
#define SET_BLE_DEVICE_NAME   "AT+NAMB"    //"AT+NAMEB[p1]"
#define SET_MODULE_BAUDTATE   "AT+BAUD"    //"AT+BAUD[P1]" p1 1~7 default 6 1-4800 2-9600 3-19200 4-38400 5-57600 6-115200
#define SET_MODULE_CONNECT_MODE "AT+DUAL"  //"AT+DUAL[p1]" p1 0~1 default 0 SPP BEL双模待机同事收发 1 双模待机单模收发
#define SET_MODULE_DATA_SPEED  "AT+HIGH"   //"AT+HIGH[p1]" p1 0~1 default 0 0-平衡模式  1-高速模式
#define SET_EDR_WORK_MODE      "AT+IMME"    //"AT+IMME[p1]" p1 0~1 de 0 0-自动工作 1-上电等待指令工作
#define SET_BLE_WORK_MODE      "AT+IMMB"    //"AT+IMMB[p1]" p1 0~1 de 0 0-自动工作 1-上电等待指令工作
#define SET_NOTIFY_CONNECT_STATUS "AT+NOTI"  //"AT+NOTI[p1]" p1 0~1 de 1 0-连接后不通知上位机 1-连接后通知上位机
#define SET_PIO1_LED_STATUS   "AT+PIO1"    //"AT+PIO1[P1]" p1 0 待机慢闪连接后常亮 1 待机不闪连接后常亮
#define SET_EDR_DEVICE_CODE   "AT+PINE"    //"AT+PINE[para1]"
#define SET_BLE_DEVICE_CODE   "AT+PINB"    //"AT+PINB[p1]"





/***蓝牙模块波特率枚举***/
enum
{
  BAUDRATE_4800 =1,
  BAUDRATE_9600 =2,
  BAUDRATE_19200 =3,
  BAUDRATE_38400 =4,
  BAUDRATE_57600 =5,
  BAUDRATE_115200 =6,
  BAUDRATE_230400 =7
}BAUDRATE_ENUM;

/***蓝牙模块连接模式枚举***/
enum
{
  CONNECT_TWO_TXRX,
  CONNECT_SINGLE_TXRX
}CONNECT_MODE_EUNM;

/***蓝牙模块数据传输速度枚举***/
enum
{
 DATA_SPEED_LOW,
 DATA_SPEED_HIGH
}DATA_TRANSER_SPEED_ENUM;

/***蓝牙模块EDR工作模式枚举***/
enum
{
 EDR_WORK_AUTO,
 EDR_WORK_MANUAL
}EDR_WORK_MODE_ENUM;

/***蓝牙模块成功连接后是否通知上位机枚举***/
enum
{
 CONNECT_NOTIFY_NO,
 CONNECT_NOTIFY_YES
}CONNECT_NOTIFY_ENUM;

/***蓝牙模块 LED 工作模式枚举***/
enum
{
 IDLE_LED_FLASH,
 IDLE_LED_OFF
}PIO1_LED_STATUS_ENUM;




int AT_Set_device_name_code(char *code);
int AT_Set_device_name_cmd(char *name);
int Init_hm_blue_module(void);
int hm_bluetooth_connect_status(void);





#endif //




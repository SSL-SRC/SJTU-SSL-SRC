/********************************************************************************
********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:    Hm_bluetooth.h         
* ��������:    
       ����ģ�鴮��ͨ��Э��ͷ�ļ�����
* �汾      ����       ʱ��          ״̬
* V1.0      sunlf      2015.10.14    �����ļ�
*****************************************************************************
*****************************************************************************/ 

#ifndef __HM_BLE_H
#define __HM_BLE_H




#define TEST_BLE              "AT"          //test mode
#define QUES_BLE_MAC          "AT+ADDB?"
#define SET_EDR_DEVICE_NAME   "AT+NAME"    //"AT+NAMEE[para1]"
#define SET_BLE_DEVICE_NAME   "AT+NAMB"    //"AT+NAMEB[p1]"
#define SET_MODULE_BAUDTATE   "AT+BAUD"    //"AT+BAUD[P1]" p1 1~7 default 6 1-4800 2-9600 3-19200 4-38400 5-57600 6-115200
#define SET_MODULE_CONNECT_MODE "AT+DUAL"  //"AT+DUAL[p1]" p1 0~1 default 0 SPP BEL˫ģ����ͬ���շ� 1 ˫ģ������ģ�շ�
#define SET_MODULE_DATA_SPEED  "AT+HIGH"   //"AT+HIGH[p1]" p1 0~1 default 0 0-ƽ��ģʽ  1-����ģʽ
#define SET_EDR_WORK_MODE      "AT+IMME"    //"AT+IMME[p1]" p1 0~1 de 0 0-�Զ����� 1-�ϵ�ȴ�ָ���
#define SET_BLE_WORK_MODE      "AT+IMMB"    //"AT+IMMB[p1]" p1 0~1 de 0 0-�Զ����� 1-�ϵ�ȴ�ָ���
#define SET_NOTIFY_CONNECT_STATUS "AT+NOTI"  //"AT+NOTI[p1]" p1 0~1 de 1 0-���Ӻ�֪ͨ��λ�� 1-���Ӻ�֪ͨ��λ��
#define SET_PIO1_LED_STATUS   "AT+PIO1"    //"AT+PIO1[P1]" p1 0 �����������Ӻ��� 1 �����������Ӻ���
#define SET_EDR_DEVICE_CODE   "AT+PINE"    //"AT+PINE[para1]"
#define SET_BLE_DEVICE_CODE   "AT+PINB"    //"AT+PINB[p1]"





/***����ģ�鲨����ö��***/
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

/***����ģ������ģʽö��***/
enum
{
  CONNECT_TWO_TXRX,
  CONNECT_SINGLE_TXRX
}CONNECT_MODE_EUNM;

/***����ģ�����ݴ����ٶ�ö��***/
enum
{
 DATA_SPEED_LOW,
 DATA_SPEED_HIGH
}DATA_TRANSER_SPEED_ENUM;

/***����ģ��EDR����ģʽö��***/
enum
{
 EDR_WORK_AUTO,
 EDR_WORK_MANUAL
}EDR_WORK_MODE_ENUM;

/***����ģ��ɹ����Ӻ��Ƿ�֪ͨ��λ��ö��***/
enum
{
 CONNECT_NOTIFY_NO,
 CONNECT_NOTIFY_YES
}CONNECT_NOTIFY_ENUM;

/***����ģ�� LED ����ģʽö��***/
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




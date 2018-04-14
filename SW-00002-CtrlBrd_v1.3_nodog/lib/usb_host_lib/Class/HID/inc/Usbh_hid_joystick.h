/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HID_JOYSTICK_H
#define __USBH_HID_JOYSTICK_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"


/***��ͨ����usb ��Ϸ�ֱ� ���ݰ�Byte0 �ṹ�嶨��***/
typedef union
{
  uint8_t all;
  struct
  {
    uint8_t Betop_X_Key:1;  //�󰴼� ����ƽ��
	uint8_t Betop_A_Key:1;  //�°���
	uint8_t Betop_B_Key:1;  //�ұ߰���  ��������
	uint8_t Betop_Y_Kye:1;  //�ϰ���   ���ڼ���
	uint8_t Betop_LB_Key:1;
	uint8_t Betop_RB_Key:1;
	uint8_t Betop_LT_Di_Key:1;
	uint8_t Betop_RT_Di_Key:1;
  }bit;
}JOYSTICK_KEY_STRUCT;

/***��ͨ����usb ��Ϸ�ֱ� ���ݰ�Byte1 �ṹ�嶨��***/
typedef union
{
  uint8_t all;
  struct
  {
    uint8_t Betop_Back_Key:1;
	uint8_t Betop_Start_Key:1;
	uint8_t reserved:2;
	uint8_t Betop_betop_Kye:1;
	uint8_t reserved1:3;
  }bit;
}JOYSTICK_START_KEY_STRUCT;

/***�ֱ����Ͻ�ҡ��***/
typedef struct
{
	uint8_t X_axis;  //x��ģ�������� 0x00~0xff �м� 0x80 ���0x00 �ұ�0xff
	uint8_t Y_axis; //y��ģ�������� 0x00~0xff �м�0x80 �ϱ�0x00 �±�0xff

}JOYSTICK_XY_STRUCT;

/****��ͨ��Ϸ�ֱ�20�ֽ����ݰ�****/
typedef struct _HID_JOYSTICK_Data
{
  JOYSTICK_KEY_STRUCT         key_button;  //byte0
  JOYSTICK_START_KEY_STRUCT   start_button;//byte1
  uint8_t                     vision_helmet_button;//byte2 
  JOYSTICK_XY_STRUCT          joystick_xy_left;   //���ҡ�� byte3~4
  JOYSTICK_XY_STRUCT          joystick_xy_right;  //�ұ�ҡ�� byte5~6
  uint8_t                     reserved[10];       //byte7~16
  uint8_t                     lt_analog_button;   //LT ģ�������� byte17
  uint8_t                     rt_analog_button;   //RT ģ�������� �ſ�0x00 ȫ������ 0xff byte18
  uint8_t                     packet_valid;            //���ݰ���Ч
  
}
HID_JOYSTICK_Data_TypeDef;



extern HID_cb_TypeDef HID_JOYSTICK_cb;
extern HID_JOYSTICK_Data_TypeDef	 HID_JOYSTICK_Data;




#endif /* __USBH_HID_JOYSTICK_H */


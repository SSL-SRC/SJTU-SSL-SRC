/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HID_JOYSTICK_H
#define __USBH_HID_JOYSTICK_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"


/***北通无线usb 游戏手柄 数据包Byte0 结构体定义***/
typedef union
{
  uint8_t all;
  struct
  {
    uint8_t Betop_X_Key:1;  //左按键 用于平射
	uint8_t Betop_A_Key:1;  //下按键
	uint8_t Betop_B_Key:1;  //右边按键  用于挑射
	uint8_t Betop_Y_Kye:1;  //上按键   用于加速
	uint8_t Betop_LB_Key:1;
	uint8_t Betop_RB_Key:1;
	uint8_t Betop_LT_Di_Key:1;
	uint8_t Betop_RT_Di_Key:1;
  }bit;
}JOYSTICK_KEY_STRUCT;

/***北通无线usb 游戏手柄 数据包Byte1 结构体定义***/
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

/***手柄左上角摇杆***/
typedef struct
{
	uint8_t X_axis;  //x轴模拟量数据 0x00~0xff 中间 0x80 左边0x00 右边0xff
	uint8_t Y_axis; //y轴模拟量数据 0x00~0xff 中间0x80 上边0x00 下边0xff

}JOYSTICK_XY_STRUCT;

/****北通游戏手柄20字节数据包****/
typedef struct _HID_JOYSTICK_Data
{
  JOYSTICK_KEY_STRUCT         key_button;  //byte0
  JOYSTICK_START_KEY_STRUCT   start_button;//byte1
  uint8_t                     vision_helmet_button;//byte2 
  JOYSTICK_XY_STRUCT          joystick_xy_left;   //左边摇杆 byte3~4
  JOYSTICK_XY_STRUCT          joystick_xy_right;  //右边摇杆 byte5~6
  uint8_t                     reserved[10];       //byte7~16
  uint8_t                     lt_analog_button;   //LT 模拟量按键 byte17
  uint8_t                     rt_analog_button;   //RT 模拟量按键 放开0x00 全部按下 0xff byte18
  uint8_t                     packet_valid;            //数据包有效
  
}
HID_JOYSTICK_Data_TypeDef;



extern HID_cb_TypeDef HID_JOYSTICK_cb;
extern HID_JOYSTICK_Data_TypeDef	 HID_JOYSTICK_Data;




#endif /* __USBH_HID_JOYSTICK_H */


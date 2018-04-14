/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Header for usbd_cdc_vcp.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"

#include "usbd_cdc_core.h"
#include "usbd_conf.h"


#define USB_RX_BUF_SIZE   64


typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;


#define DEFAULT_CONFIG                  0
#define OTHER_CONFIG                    1

/***usb下发参数设置类型 debug模式支持的类型***/
enum
{
	SET_BLUETOOTH_NAME =0x01,  //设置蓝牙名称 返回1Byte
	SET_BLUETOOTH_CODE,        //设置蓝牙密码 返回1Byte
	SET_MOTOR_PID,
	GET_CONTROL_STATUS,         //获取控制器状态 返回3Byte
	GET_CPU_ID,                 //获取cpuid 96bit
	SET_UNLOCK_FALSH_RDPROTECTION,   //解除控制器falsh读保护设置  flash程序擦除 谨慎操作
	GET_CAP_VOLATAGE,            //用于测试模式获取充电电容电压 电池电压
	SET_ROBOT_DO_SHOOT          //设置机器人执行射门动作   
	
	
};

extern uint8_t USB_Receive_Buffer[USB_RX_BUF_SIZE];
extern uint8_t USB_Receive_ok;
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.发送给主机的缓存
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __USBD_CDC_VCP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

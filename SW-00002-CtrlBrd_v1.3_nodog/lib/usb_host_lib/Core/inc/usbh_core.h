/**
  ******************************************************************************
  * @file    usbh_core.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   Header file for usbh_core.c
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

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_CORE_H
#define __USBH_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "usb_hcd.h"
#include "usbh_def.h"
#include "usbh_conf.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_CORE
  * @brief This file is the Header file for usbh_core.c
  * @{
  */ 


/** @defgroup USBH_CORE_Exported_Defines
  * @{
  */ 

#define MSC_CLASS                         0x08
#define HID_CLASS                         0x03
#define MSC_PROTOCOL                      0x50
#define CBI_PROTOCOL                      0x01


#define USBH_MAX_ERROR_COUNT                            2
#define USBH_DEVICE_ADDRESS_DEFAULT                     0
#define USBH_DEVICE_ADDRESS                             1


/**
  * @}
  */ 


/** @defgroup USBH_CORE_Exported_Types
  * @{
  */

typedef enum {
  USBH_OK   = 0,
  USBH_BUSY,
  USBH_FAIL,
  USBH_NOT_SUPPORTED,
  USBH_UNRECOVERED_ERROR,
  USBH_ERROR_SPEED_UNKNOWN,
  USBH_APPLY_DEINIT
}USBH_Status;

/* Following states are used for gState */
typedef enum {
  HOST_IDLE =0,                        //usb host  处于空闲状态
  HOST_DEV_ATTACHED,                   //设备挂接上
  HOST_DEV_DISCONNECTED,               //设备断开状态
  HOST_DETECT_DEVICE_SPEED,            //检测设备速度
  HOST_ENUMERATION,                    //枚举状态 挂接上后一个状态
  HOST_CLASS_REQUEST,                   //子设备类请求
  HOST_CLASS,                           //子设备处理状态
  HOST_CTRL_XFER,                      //控制数据传输状态
  HOST_USR_INPUT,                     //用户输入 枚举成功后的下一状态
  HOST_SUSPENDED,
  HOST_ERROR_STATE  
}HOST_State;  

/* Following states are used for EnumerationState */
typedef enum {
  ENUM_IDLE = 0,                        //枚举空闲状态
  ENUM_GET_FULL_DEV_DESC,               //获取设备描述符
  ENUM_SET_ADDR,                        //设置设备地址
  ENUM_GET_CFG_DESC,                    //获取配置描述符
  ENUM_GET_FULL_CFG_DESC,               //获取全部配置描述符
  ENUM_GET_MFC_STRING_DESC,
  ENUM_GET_PRODUCT_STRING_DESC,
  ENUM_GET_SERIALNUM_STRING_DESC,
  ENUM_SET_CONFIGURATION,
  ENUM_DEV_CONFIGURED                    //已经完成配置
} ENUM_State;  



/* Following states are used for CtrlXferStateMachine */
typedef enum {
  CTRL_IDLE =0,
  CTRL_SETUP,          //控制建立阶段
  CTRL_SETUP_WAIT,     //控制传输后等待
  CTRL_DATA_IN,        //控制包数据传输完成 数据输入阶段
  CTRL_DATA_IN_WAIT,
  CTRL_DATA_OUT,       //控制包数据传输完成 数据输出阶段
  CTRL_DATA_OUT_WAIT,
  CTRL_STATUS_IN,
  CTRL_STATUS_IN_WAIT,
  CTRL_STATUS_OUT,
  CTRL_STATUS_OUT_WAIT,
  CTRL_ERROR,
  CTRL_STALLED,
  CTRL_COMPLETE    
}
CTRL_State;  

typedef enum {
  USBH_USR_NO_RESP   = 0,
  USBH_USR_RESP_OK = 1,
}
USBH_USR_Status;

/* Following states are used for RequestState */
typedef enum {
  CMD_IDLE =0,
  CMD_SEND,           
  CMD_WAIT
} CMD_State;  



typedef struct _Ctrl
{
  uint8_t               hc_num_in;     //控制管道 通道num号 IN端点   0~8
  uint8_t               hc_num_out;   //通道num号 OUT端点
  uint8_t               ep0size;      //控制端点0的大小
  uint8_t               *buff;        //指向接收数据buf
  uint16_t              length;       //数据长度
  uint8_t               errorcount;
  uint16_t              timer;        //控制传输timeout 时间变量 帧编号时间
  CTRL_STATUS           status;
  USB_Setup_TypeDef     setup;   //setup packet 8Byte 标准设备请求数据包
  CTRL_State            state;   //控制状态

} USBH_Ctrl_TypeDef;


/***USB 主机获取到的设备参数***/
typedef struct _DeviceProp
{
  
  uint8_t                           address;     //设备默认地址设置为0
  uint8_t                           speed;      //设备速度 00 高速 01 全速 10 低速
  USBH_DevDesc_TypeDef              Dev_Desc;   //设备描述符
  USBH_CfgDesc_TypeDef              Cfg_Desc;   //配置描述符
  USBH_InterfaceDesc_TypeDef        Itf_Desc[USBH_MAX_NUM_INTERFACES];//接口描述符
  USBH_EpDesc_TypeDef               Ep_Desc[USBH_MAX_NUM_INTERFACES][USBH_MAX_NUM_ENDPOINTS];//一个接口对应一个或者多个端点 端点描述符
  USBH_HIDDesc_TypeDef              HID_Desc;
  
}USBH_Device_TypeDef;

typedef struct _USBH_Class_cb
{
  USBH_Status  (*Init)(USB_OTG_CORE_HANDLE *pdev , void *phost);
    
  void   (*DeInit)(USB_OTG_CORE_HANDLE *pdev , void *phost);
    
  USBH_Status  (*Requests)(USB_OTG_CORE_HANDLE *pdev , void *phost); 
     
  USBH_Status  (*Machine)(USB_OTG_CORE_HANDLE *pdev , void *phost);  
      
  
} USBH_Class_cb_TypeDef;


typedef struct _USBH_USR_PROP
{
  void (*Init)(void);       /* HostLibInitialized */
  void (*DeInit)(void);       /* HostLibInitialized */  
  void (*DeviceAttached)(void);           /* DeviceAttached */
  void (*ResetDevice)(void);
  void (*DeviceDisconnected)(void); 
  void (*OverCurrentDetected)(void);  
  void (*DeviceSpeedDetected)(uint8_t DeviceSpeed);          /* DeviceSpeed */
  void (*DeviceDescAvailable)(void *);    /* DeviceDescriptor is available */
  void (*DeviceAddressAssigned)(void);  /* Address is assigned to USB Device */
  void (*ConfigurationDescAvailable)(USBH_CfgDesc_TypeDef *,
                                     USBH_InterfaceDesc_TypeDef *,
                                     USBH_EpDesc_TypeDef *); 
  /* Configuration Descriptor available */
  void (*ManufacturerString)(void *);     /* ManufacturerString*/
  void (*ProductString)(void *);          /* ProductString*/
  void (*SerialNumString)(void *);        /* SerialNubString*/
  void (*EnumerationDone)(void);           /* Enumeration finished */
  USBH_USR_Status (*UserInput)(void);
  int  (*UserApplication) (void);
  void (*DeviceNotSupported)(void); /* Device is not supported*/
  void (*UnrecoveredError)(void);

}
USBH_Usr_cb_TypeDef;

typedef struct _Host_TypeDef
{
  HOST_State            gState;       /*  Host State Machine Value */
  HOST_State            gStateBkp;    /* backup of previous State machine value */
  ENUM_State            EnumState;    /* Enumeration state Machine */
  CMD_State             RequestState;    //请求状态    
  USBH_Ctrl_TypeDef     Control;     //控制传输结构
  
  USBH_Device_TypeDef   device_prop;  //关于设备的结构体变量
  
  USBH_Class_cb_TypeDef               *class_cb;   //指向class 子类处理函数的指针
  USBH_Usr_cb_TypeDef  	              *usr_cb;  //指向用户执行的函数指针

  
} USBH_HOST, *pUSBH_HOST;

/**
  * @}
  */ 



/** @defgroup USBH_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_CORE_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_CORE_Exported_FunctionsPrototype
  * @{
  */ 

extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;
extern __ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;


void USBH_Init(USB_OTG_CORE_HANDLE *pdev,
               USB_OTG_CORE_ID_TypeDef coreID, 
               USBH_HOST *phost,                    
               USBH_Class_cb_TypeDef *class_cb, 
               USBH_Usr_cb_TypeDef *usr_cb);
               
USBH_Status USBH_DeInit(USB_OTG_CORE_HANDLE *pdev, 
                        USBH_HOST *phost);
void USBH_Process(USB_OTG_CORE_HANDLE *pdev , 
                  USBH_HOST *phost);
void USBH_ErrorHandle(USBH_HOST *phost, 
                      USBH_Status errType);

/**
  * @}
  */ 

#endif /* __USBH_CORE_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




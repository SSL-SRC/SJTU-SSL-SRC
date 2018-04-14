/**
  ******************************************************************************
  * @file    usb_core.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   Header of the Core Layer
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
#ifndef __USB_CORE_H__
#define __USB_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
#include "usb_regs.h"
#include "usb_defines.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_CORE
  * @brief usb otg driver core layer
  * @{
  */ 


/** @defgroup USB_CORE_Exported_Defines
  * @{
  */ 

#define USB_OTG_EP0_IDLE                          0
#define USB_OTG_EP0_SETUP                         1
#define USB_OTG_EP0_DATA_IN                       2
#define USB_OTG_EP0_DATA_OUT                      3
#define USB_OTG_EP0_STATUS_IN                     4
#define USB_OTG_EP0_STATUS_OUT                    5
#define USB_OTG_EP0_STALL                         6

#define USB_OTG_EP_TX_DIS       0x0000
#define USB_OTG_EP_TX_STALL     0x0010
#define USB_OTG_EP_TX_NAK       0x0020
#define USB_OTG_EP_TX_VALID     0x0030
 
#define USB_OTG_EP_RX_DIS       0x0000
#define USB_OTG_EP_RX_STALL     0x1000
#define USB_OTG_EP_RX_NAK       0x2000
#define USB_OTG_EP_RX_VALID     0x3000
/**
  * @}
  */ 
#define   MAX_DATA_LENGTH                        0x200    //512 RXFIFO大小设置为128*4=512Byte

/** @defgroup USB_CORE_Exported_Types
  * @{
  */ 


typedef enum {
  USB_OTG_OK = 0,
  USB_OTG_FAIL
}USB_OTG_STS;

/*主机通道状态*/
typedef enum {
  HC_IDLE = 0,
  HC_XFRC,                  //数据传输完成
  HC_HALTED,
  HC_NAK,                   //收到NAK响应
  HC_NYET,                  //收到NYTE响应
  HC_STALL,                 //收到stall响应
  HC_XACTERR,               //通信事务错误 CRC校验失败或超时  位填充错误 错误的EOP
  HC_BBLERR,                //串扰错误
  HC_DATATGLERR,            //数据同步错误
}HC_STATUS;

typedef enum {
  URB_IDLE = 0,
  URB_DONE,
  URB_NOTREADY,
  URB_ERROR,
  URB_STALL
}URB_STATE;

typedef enum {
  CTRL_START = 0,
  CTRL_XFRC,
  CTRL_HALTED,
  CTRL_NAK,
  CTRL_STALL,
  CTRL_XACTERR,  
  CTRL_BBLERR,   
  CTRL_DATATGLERR,  
  CTRL_FAIL
}CTRL_STATUS;


typedef struct USB_OTG_hc
{
  uint8_t       dev_addr ;           //主机给设备分配的地址
  uint8_t       ep_num;              //端点地址 最大4bit 0~15
  uint8_t       ep_is_in;            //输入端点标志位 bit7为1表示IN
  uint8_t       speed;               //速度 00 高速 01 全速 10 低速
  uint8_t       do_ping;             //HS 则为1
  uint8_t       ep_type;             //端点类型
  uint16_t      max_packet;          //最大包大小
  uint8_t       data_pid;            //传输的数据PID 00 data0 01 data2 10 data1 11 mdata
  uint8_t       *xfer_buff;          //要传输或者接收的BUF地址
  uint32_t      xfer_len;            //传输的数据大小
  uint32_t      xfer_count;  
  uint8_t       toggle_in;           //DATA0 DATA1 标志
  uint8_t       toggle_out;
  uint32_t       dma_addr;  
}
USB_OTG_HC , *PUSB_OTG_HC;

/***端点相关结构体***/
typedef struct USB_OTG_ep
{
  uint8_t        num;                //端点号 0~4 example 最大16个端点
  uint8_t        is_in;             //1表示输入端点 0表示输出端点
  uint8_t        is_stall;  
  uint8_t        type;              //端点类型 控制传输类型
  uint8_t        data_pid_start;
  uint8_t        even_odd_frame;
  uint16_t       tx_fifo_num;        //发送端点号fifo
  uint32_t       maxpacket;          //端点最大数据包大小
  /* transaction level variables*/ 
  uint8_t        *xfer_buff;         //传输/接收数据地址
  uint32_t       dma_addr;           //dma传输地址
  uint32_t       xfer_len;           //要传输的数据长度
  uint32_t       xfer_count;         //已经传输/接收的数据长度
  /* Transfer level variables*/  
  uint32_t       rem_data_len;       //还需要传输数据长度
  uint32_t       total_data_len;
  uint32_t       ctl_data_len;  

}

USB_OTG_EP , *PUSB_OTG_EP;




typedef struct USB_OTG_core_cfg
{
  uint8_t       host_channels;                //主机通道个数  FS 8个 HS 12个
  uint8_t       dev_endpoints;                //设备端点个数   FS 3个in 3个OUT 一个控制  设备4个端点 主机8个端点 
  uint8_t       speed;                        //HS 5个IN 5个OUT 一个控制   设备 6个端点 主机12个端点
  uint8_t       dma_enable;
  uint16_t      mps;                          //最大packet size
  uint16_t      TotalFifoSize;
  uint8_t       phy_itface;
  uint8_t       Sof_output;                  //sof  输出使能标志位
  uint8_t       low_power;
  uint8_t       coreID;
 
}
USB_OTG_CORE_CFGS, *PUSB_OTG_CORE_CFGS;



typedef  struct  usb_setup_req {
    
    uint8_t   bmRequest;                      
    uint8_t   bRequest;                           
    uint16_t  wValue;                             
    uint16_t  wIndex;                             
    uint16_t  wLength;                            
} USB_SETUP_REQ;

typedef struct _Device_TypeDef
{
  uint8_t  *(*GetDeviceDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetLangIDStrDescriptor)( uint8_t speed , uint16_t *length); 
  uint8_t  *(*GetManufacturerStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetProductStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetSerialStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetConfigurationStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetInterfaceStrDescriptor)( uint8_t speed , uint16_t *length);   
} USBD_DEVICE, *pUSBD_DEVICE;

//typedef struct USB_OTG_hPort
//{
//  void (*Disconnect) (void *phost);
//  void (*Connect) (void *phost); 
//  uint8_t ConnStatus;
//  uint8_t DisconnStatus;
//  uint8_t ConnHandled;
//  uint8_t DisconnHandled;
//} USB_OTG_hPort_TypeDef;

typedef struct _Device_cb
{
  uint8_t  (*Init)         (void *pdev , uint8_t cfgidx); //设备子类初始化
  uint8_t  (*DeInit)       (void *pdev , uint8_t cfgidx);
 /* Control Endpoints*/
  uint8_t  (*Setup)        (void *pdev , USB_SETUP_REQ  *req);   //设备子类setup控制包解析
  uint8_t  (*EP0_TxSent)   (void *pdev );    
  uint8_t  (*EP0_RxReady)  (void *pdev );  
  /* Class Specific Endpoints*/
  uint8_t  (*DataIn)       (void *pdev , uint8_t epnum);    //处理将要发送的数据给PC机 
  uint8_t  (*DataOut)      (void *pdev , uint8_t epnum);    //处理从PC机获取的数据
  uint8_t  (*SOF)          (void *pdev); 
  uint8_t  (*IsoINIncomplete)  (void *pdev); 
  uint8_t  (*IsoOUTIncomplete)  (void *pdev);   

  uint8_t  *(*GetConfigDescriptor)( uint8_t speed , uint16_t *length); 
#ifdef USB_OTG_HS_CORE 
  uint8_t  *(*GetOtherConfigDescriptor)( uint8_t speed , uint16_t *length);   
#endif

#ifdef USB_SUPPORT_USER_STRING_DESC 
  uint8_t  *(*GetUsrStrDescriptor)( uint8_t speed ,uint8_t index,  uint16_t *length);   
#endif  
  
} USBD_Class_cb_TypeDef;



typedef struct _USBD_USR_PROP
{
  void (*Init)(void);   
  void (*DeviceReset)(uint8_t speed); 
  void (*DeviceConfigured)(void);
  void (*DeviceSuspended)(void);
  void (*DeviceResumed)(void);  
  
  void (*DeviceConnected)(void);  
  void (*DeviceDisconnected)(void);    
  
}
USBD_Usr_cb_TypeDef;

/***设备驱动结构体***/
typedef struct _DCD
{
  uint8_t        device_config;
  uint8_t        device_state;        //设备状态 USB_OTG_EP0_SETUP
  uint8_t        device_status;       //设备状态
  uint8_t        device_old_status;
  uint8_t        device_address;      //设备地址
  uint8_t        connection_status;   //设备连接标志
  uint8_t        test_mode;
  uint32_t       DevRemoteWakeup;
  USB_OTG_EP     in_ep   [USB_OTG_MAX_TX_FIFOS];          //输入端点 usb协议规定端点域4bit 最多16个端点
  USB_OTG_EP     out_ep  [USB_OTG_MAX_TX_FIFOS];          //输出端点 PC机发数据给设备
  uint8_t        setup_packet [8*3];
  USBD_Class_cb_TypeDef         *class_cb; //指向设备子类结构体
  USBD_Usr_cb_TypeDef           *usr_cb;   //指向用户设备结构体
  USBD_DEVICE                   *usr_device;  
  uint8_t        *pConfig_descriptor;
 }
DCD_DEV , *DCD_PDEV;



typedef struct _HCD
{
  uint8_t                  Rx_Buffer [MAX_DATA_LENGTH];   //接收数据buf 
  __IO uint32_t            ConnSts;                       //usb 连接标志位   1 表示usb 连接  0表示未连接
  __IO uint32_t            ErrCnt[USB_OTG_MAX_TX_FIFOS];  //对应通道错误计数
  __IO uint32_t            XferCnt[USB_OTG_MAX_TX_FIFOS];
  __IO HC_STATUS           HC_Status[USB_OTG_MAX_TX_FIFOS];    //指定通道号的状态
  __IO URB_STATE           URB_State[USB_OTG_MAX_TX_FIFOS];
  USB_OTG_HC               hc [USB_OTG_MAX_TX_FIFOS];      //主机通道号 FS 8个通道 HS 12个通道
  uint16_t                 channel [USB_OTG_MAX_TX_FIFOS];    //通道号 端点号 值
//  USB_OTG_hPort_TypeDef    *port_cb;  
}
HCD_DEV , *USB_OTG_USBH_PDEV;


typedef struct _OTG
{
  uint8_t    OTG_State;
  uint8_t    OTG_PrevState;  
  uint8_t    OTG_Mode;    
}
OTG_DEV , *USB_OTG_USBO_PDEV;

typedef struct USB_OTG_handle
{
  USB_OTG_CORE_CFGS    cfg;    //配置寄存器用于根据这些参数设置相应寄存器
  USB_OTG_CORE_REGS    regs;
#ifdef USE_DEVICE_MODE
  DCD_DEV     dev;
#endif
#ifdef USE_HOST_MODE
  HCD_DEV     host;   //主机参数
#endif
#ifdef USE_OTG_MODE
  OTG_DEV     otg;
#endif
}
USB_OTG_CORE_HANDLE , *PUSB_OTG_CORE_HANDLE;

/**
  * @}
  */ 


/** @defgroup USB_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_FunctionsPrototype
  * @{
  */ 


USB_OTG_STS  USB_OTG_CoreInit        (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_SelectCore      (USB_OTG_CORE_HANDLE *pdev, 
                                      USB_OTG_CORE_ID_TypeDef coreID);
USB_OTG_STS  USB_OTG_EnableGlobalInt (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_DisableGlobalInt(USB_OTG_CORE_HANDLE *pdev);
void*           USB_OTG_ReadPacket   (USB_OTG_CORE_HANDLE *pdev ,
    uint8_t *dest,
    uint16_t len);
USB_OTG_STS  USB_OTG_WritePacket     (USB_OTG_CORE_HANDLE *pdev ,
    uint8_t *src,
    uint8_t ch_ep_num,
    uint16_t len);
USB_OTG_STS  USB_OTG_FlushTxFifo     (USB_OTG_CORE_HANDLE *pdev , uint32_t num);
USB_OTG_STS  USB_OTG_FlushRxFifo     (USB_OTG_CORE_HANDLE *pdev);

uint32_t     USB_OTG_ReadCoreItr     (USB_OTG_CORE_HANDLE *pdev);
uint32_t     USB_OTG_ReadOtgItr      (USB_OTG_CORE_HANDLE *pdev);
uint8_t      USB_OTG_IsHostMode      (USB_OTG_CORE_HANDLE *pdev);
uint8_t      USB_OTG_IsDeviceMode    (USB_OTG_CORE_HANDLE *pdev);
uint32_t     USB_OTG_GetMode         (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_PhyInit         (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_SetCurrentMode  (USB_OTG_CORE_HANDLE *pdev,
    uint8_t mode);

/*********************** HOST APIs ********************************************/
#ifdef USE_HOST_MODE
USB_OTG_STS  USB_OTG_CoreInitHost    (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EnableHostInt   (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_HC_Init         (USB_OTG_CORE_HANDLE *pdev, uint8_t hc_num);
USB_OTG_STS  USB_OTG_HC_Halt         (USB_OTG_CORE_HANDLE *pdev, uint8_t hc_num);
USB_OTG_STS  USB_OTG_HC_StartXfer    (USB_OTG_CORE_HANDLE *pdev, uint8_t hc_num);
USB_OTG_STS  USB_OTG_HC_DoPing       (USB_OTG_CORE_HANDLE *pdev , uint8_t hc_num);
uint32_t     USB_OTG_ReadHostAllChannels_intr    (USB_OTG_CORE_HANDLE *pdev);
uint32_t     USB_OTG_ResetPort       (USB_OTG_CORE_HANDLE *pdev);
uint32_t     USB_OTG_ReadHPRT0       (USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_DriveVbus       (USB_OTG_CORE_HANDLE *pdev, uint8_t state);
void         USB_OTG_InitFSLSPClkSel (USB_OTG_CORE_HANDLE *pdev ,uint8_t freq);
uint8_t      USB_OTG_IsEvenFrame     (USB_OTG_CORE_HANDLE *pdev) ;
void         USB_OTG_StopHost        (USB_OTG_CORE_HANDLE *pdev);
#endif
/********************* DEVICE APIs ********************************************/
#ifdef USE_DEVICE_MODE
USB_OTG_STS  USB_OTG_CoreInitDev         (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EnableDevInt        (USB_OTG_CORE_HANDLE *pdev);
uint32_t     USB_OTG_ReadDevAllInEPItr           (USB_OTG_CORE_HANDLE *pdev);
enum USB_OTG_SPEED USB_OTG_GetDeviceSpeed (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EP0Activate (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EPActivate  (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPDeactivate(USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPStartXfer (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EP0StartXfer(USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPSetStall          (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPClearStall        (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
uint32_t     USB_OTG_ReadDevAllOutEp_itr (USB_OTG_CORE_HANDLE *pdev);
uint32_t     USB_OTG_ReadDevOutEP_itr    (USB_OTG_CORE_HANDLE *pdev , uint8_t epnum);
uint32_t     USB_OTG_ReadDevAllInEPItr   (USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_InitDevSpeed        (USB_OTG_CORE_HANDLE *pdev , uint8_t speed);
uint8_t      USBH_IsEvenFrame (USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_EP0_OutStart(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_ActiveRemoteWakeup(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_UngateClock(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_StopDevice(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_SetEPStatus (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep , uint32_t Status);
uint32_t     USB_OTG_GetEPStatus(USB_OTG_CORE_HANDLE *pdev ,USB_OTG_EP *ep);
#endif
/**
  * @}
  */ 

#endif  /* __USB_CORE_H__ */


/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


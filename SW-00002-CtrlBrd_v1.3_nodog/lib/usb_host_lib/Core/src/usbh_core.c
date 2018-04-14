/**
  ******************************************************************************
  * @file    usbh_core.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file implements the functions for the core state machine process
  *          the enumeration and the control transfer process
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
/* Includes ------------------------------------------------------------------*/

#include "usbh_ioreq.h"
#include "usb_bsp.h"
#include "usbh_hcs.h"
#include "usbh_stdreq.h"
#include "usbh_core.h"
#include "usb_hcd_int.h"

#include "gpio.h"





#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;



/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/

/** @defgroup USBH_CORE 
  * @brief TThis file handles the basic enumaration when a device is connected 
  *          to the host.
  * @{
  */ 

/** @defgroup USBH_CORE_Private_TypesDefinitions
  * @{
  */ 
uint8_t USBH_Disconnected (USB_OTG_CORE_HANDLE *pdev); 
uint8_t USBH_Connected (USB_OTG_CORE_HANDLE *pdev); 
uint8_t USBH_SOF (USB_OTG_CORE_HANDLE *pdev); 

USBH_HCD_INT_cb_TypeDef USBH_HCD_INT_cb = 
{
  USBH_SOF,
  USBH_Connected, 
  USBH_Disconnected,    
};

USBH_HCD_INT_cb_TypeDef  *USBH_HCD_INT_fops = &USBH_HCD_INT_cb;
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_FunctionPrototypes
  * @{
  */
static USBH_Status USBH_HandleEnum(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost);
USBH_Status USBH_HandleControl (USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost);

/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Functions
  * @{
  */ 


/**
  * @brief  USBH_Connected
  *         USB Connect callback function from the Interrupt. 
  * @param  selected device
  * @retval Status
*
usb 已经连接上
*/
uint8_t USBH_Connected (USB_OTG_CORE_HANDLE *pdev)
{
  pdev->host.ConnSts = 1;
  return 0;
}

/**
* @brief  USBH_Disconnected
*         USB Disconnect callback function from the Interrupt. 
* @param  selected device
* @retval Status
*/

uint8_t USBH_Disconnected (USB_OTG_CORE_HANDLE *pdev)
{
  pdev->host.ConnSts = 0;
  return 0;  
}

/**
  * @brief  USBH_SOF
  *         USB SOF callback function from the Interrupt. 
  * @param  selected device
  * @retval Status
  */

uint8_t USBH_SOF (USB_OTG_CORE_HANDLE *pdev)
{
  /* This callback could be used to implement a scheduler process */
  return 0;  
}
/**
  * @brief  USBH_Init
  *         Host hardware and stack initializations 
  * @param  class_cb: Class callback structure address
  * @param  usr_cb: User callback structure address
  * @retval None
  */
void USBH_Init(USB_OTG_CORE_HANDLE *pdev,
               USB_OTG_CORE_ID_TypeDef coreID,
               USBH_HOST *phost,               
               USBH_Class_cb_TypeDef *class_cb, 
               USBH_Usr_cb_TypeDef *usr_cb)
{
     
  /* Hardware Init */
  USB_OTG_BSP_Init(pdev);  
  
  /* configure GPIO pin used for switching VBUS power */
  USB_OTG_BSP_ConfigVBUS(0);  
  
  
  /* Host de-initializations */
  USBH_DeInit(pdev, phost);
  
  /*Register class and user callbacks */
  phost->class_cb = class_cb;
  phost->usr_cb = usr_cb;  
    
  /* Start the USB OTG core */     
   HCD_Init(pdev , coreID);
   
  /* Upon Init call usr call back */
  phost->usr_cb->Init();
  
  /* Enable Interrupts */
  USB_OTG_BSP_EnableInterrupt(pdev);  //NVIC 使能USB 中断
}

/**
  * @brief  USBH_DeInit 
  *         Re-Initialize Host
  * @param  None 
  * @retval status: USBH_Status
  主机状态初始化
  */
USBH_Status USBH_DeInit(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  /* Software Init */
  
  phost->gState = HOST_IDLE;      // USB HOST 初始状态为空闲状态
  phost->gStateBkp = HOST_IDLE;   //主机前一次的状态
  phost->EnumState = ENUM_IDLE;   //初始化枚举状态为空闲
  phost->RequestState = CMD_SEND;  
  //phost->RequestState = CMD_IDLE;  
  
  phost->Control.state = CTRL_SETUP;    //SETEP 包
	//phost->Control.state = CTRL_IDLE;    //SETEP 包
  phost->Control.ep0size = USB_OTG_MAX_EP0_SIZE;  
 
  phost->device_prop.address = USBH_DEVICE_ADDRESS_DEFAULT;
  phost->device_prop.speed = HPRT0_PRTSPD_FULL_SPEED;
  
  USBH_Free_Channel  (pdev, phost->Control.hc_num_in);   //释放对应通道
  USBH_Free_Channel  (pdev, phost->Control.hc_num_out);  
  return USBH_OK;
}

/**
* @brief  USBH_Process
*         USB Host core main state machine process
* @param  None 
* @retval None
*
USB 主机状态机处理
*/
void USBH_Process(USB_OTG_CORE_HANDLE *pdev , USBH_HOST *phost)
{
  volatile USBH_Status status = USBH_FAIL;
  
  
  /* check for Host port events */
  if ((HCD_IsDeviceConnected(pdev) == 0)&& (phost->gState != HOST_IDLE)) 
  {
    if(phost->gState != HOST_DEV_DISCONNECTED) 
    {
      phost->gState = HOST_DEV_DISCONNECTED;
			
    }
  }
    
  switch (phost->gState)
  {
  
  case HOST_IDLE :
    
    if (HCD_IsDeviceConnected(pdev))  //设备连接上则从空闲状态变为设备挂接上
    {
      phost->gState = HOST_DEV_ATTACHED;
      USB_OTG_BSP_mDelay(100);
    }
    break;
   
  case HOST_DEV_ATTACHED :
    
    phost->usr_cb->DeviceAttached();   //打印设备已经挂接上 可要可不要
    phost->Control.hc_num_out = USBH_Alloc_Channel(pdev, 0x00);   //0x00 0x80 端点地址 获取通道号 0 0~8 chanel[0]= 0x8000
    phost->Control.hc_num_in = USBH_Alloc_Channel(pdev, 0x80);    //获取通道号 1  chanel[1]=0x8080 bit7为1表示输入
                                                                  //host.channel[0]=0X8000 host.channel[1]=0X8080 
    /* Reset USB Device */
    if ( HCD_ResetPort(pdev) == 0)//usb 挂接后 主机发出复位信号 等待至少10ms 以上
    {
      phost->usr_cb->ResetDevice();  //用户程序 可要可不要
      /*  Wait for USB USBH_ISR_PrtEnDisableChange()
      等待HRPT中的使能非使能端口变化中断
      Host is Now ready to start the Enumeration 
      */
      
      phost->device_prop.speed = HCD_GetCurrentSpeed(pdev); //获取设备的枚举速度
      
      phost->gState = HOST_ENUMERATION;    //状态从挂接改变为枚举状态
      phost->usr_cb->DeviceSpeedDetected(phost->device_prop.speed);//用于显示连接上的usb 设备速度
        
      /* Open Control pipes 打开控制管道 IN端点 地址0x80  端点0*/
      USBH_Open_Channel (pdev,
                           phost->Control.hc_num_in,
                           phost->device_prop.address,
                           phost->device_prop.speed,
                           EP_TYPE_CTRL,
                           phost->Control.ep0size); 
      
      /* Open Control pipes 打开控制管道 OUT端点 地址0x00 bit7 1 IN 0 OUT bit0~3 端点号  端点0*/
      USBH_Open_Channel (pdev,
                           phost->Control.hc_num_out,
                           phost->device_prop.address,
                           phost->device_prop.speed,
                           EP_TYPE_CTRL,
                           phost->Control.ep0size);          
   }
    break;
    
  case HOST_ENUMERATION:       //枚举状态
    /* Check for enumeration status */  
    if ( USBH_HandleEnum(pdev , phost) == USBH_OK)
    { 
      /* The function shall return USBH_OK when full enumeration is complete */
      
      /* user callback for end of device basic enumeration */
      phost->usr_cb->EnumerationDone();
     
      phost->gState  = HOST_USR_INPUT;    //状态从枚举改变为用户输入
    }
    break;
    
  case HOST_USR_INPUT:        //用户按键按下   需要修改
    /*The function should return user response true to move to class state */
    if ( phost->usr_cb->UserInput() == USBH_USR_RESP_OK)
    {
      if((phost->class_cb->Init(pdev, phost))== USBH_OK)                     //子设备类初始化 
        
      {
        phost->gState  = HOST_CLASS_REQUEST;     
      }     
    }   
    break;
    
  case HOST_CLASS_REQUEST:  
    /* process class standard contol requests state machine */ 
    status = phost->class_cb->Requests(pdev, phost);
    
     if(status == USBH_OK)
     {
       phost->gState  = HOST_CLASS;
     }  
     
     else
     {
       USBH_ErrorHandle(phost, status);
     }
 
    
    break;    
  case HOST_CLASS:     
    /* process class state machine   处理hid handle 处理hid 数据*/
    status = phost->class_cb->Machine(pdev, phost);
    USBH_ErrorHandle(phost, status);
    break;       
    
  case HOST_CTRL_XFER:   //控制传输中
    /* process control transfer state machine */
    USBH_HandleControl(pdev, phost);    
    break;
    
  case HOST_SUSPENDED:
    break;
  
  case HOST_ERROR_STATE:
    /* Re-Initilaize Host for new Enumeration */
    USBH_DeInit(pdev, phost);
    phost->usr_cb->DeInit();
    phost->class_cb->DeInit(pdev, &phost->device_prop);
    break;
    
  case HOST_DEV_DISCONNECTED :
    
    /* Manage User disconnect operations*/
    phost->usr_cb->DeviceDisconnected();
    COMM_LED_OFF();
    /* Re-Initilaize Host for new Enumeration */
    USBH_DeInit(pdev, phost);
    phost->usr_cb->DeInit();
    phost->class_cb->DeInit(pdev, &phost->device_prop); 
    USBH_DeAllocate_AllChannel(pdev);  
    phost->gState = HOST_IDLE;
    
    break;
    
  default :
    break;
  }

}


/**
  * @brief  USBH_ErrorHandle 
  *         This function handles the Error on Host side.
  * @param  errType : Type of Error or Busy/OK state
  * @retval None
  */
void USBH_ErrorHandle(USBH_HOST *phost, USBH_Status errType)
{
  /* Error unrecovered or not supported device speed */
  if ( (errType == USBH_ERROR_SPEED_UNKNOWN) ||
       (errType == USBH_UNRECOVERED_ERROR) )
  {
    phost->usr_cb->UnrecoveredError(); 
    phost->gState = HOST_ERROR_STATE;   
  }  
  /* USB host restart requested from application layer */
  else if(errType == USBH_APPLY_DEINIT)
  {
    phost->gState = HOST_ERROR_STATE;  
    /* user callback for initalization */
    phost->usr_cb->Init();
  } 
}


/**
  * @brief  USBH_HandleEnum 
  *         This function includes the complete enumeration process
  * @param  pdev: Selected device
  * @retval USBH_Status
  usb 主机对usb 设备进行枚举
  */
static USBH_Status USBH_HandleEnum(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  USBH_Status Status = USBH_BUSY;  
  uint8_t Local_Buffer[64];
  
  switch (phost->EnumState)
  {
  case ENUM_IDLE:    //枚举空闲状态
    /* Get Device Desc for only 1st 8 bytes : To get EP0 MaxPacketSize */
    if ( USBH_Get_DevDesc(pdev , phost, 8) == USBH_OK) //获取设备描述符前8个字节
    {
      phost->Control.ep0size = phost->device_prop.Dev_Desc.bMaxPacketSize;
      
      /* Issue Reset  */
      HCD_ResetPort(pdev);
      phost->EnumState = ENUM_GET_FULL_DEV_DESC;
      
      /* modify control channels configuration for MaxPacket size */
      USBH_Modify_Channel (pdev,
                           phost->Control.hc_num_out,
                           0,
                           0,
                           0,
                           phost->Control.ep0size);
      
      USBH_Modify_Channel (pdev,
                           phost->Control.hc_num_in,
                           0,
                           0,
                           0,
                           phost->Control.ep0size);      
    }
    break;
    
  case ENUM_GET_FULL_DEV_DESC:  //获取全部的设备描述符
    /* Get FULL Device Desc  */
    if ( USBH_Get_DevDesc(pdev, phost, USB_DEVICE_DESC_SIZE)\
      == USBH_OK)
    {
      /* user callback for device descriptor available */
      phost->usr_cb->DeviceDescAvailable(&phost->device_prop.Dev_Desc);      
      phost->EnumState = ENUM_SET_ADDR;
    }
    break;
   
  case ENUM_SET_ADDR:  //设置设备地址
    /* set address */
    if ( USBH_SetAddress(pdev, phost, USBH_DEVICE_ADDRESS) == USBH_OK)
    {
      USB_OTG_BSP_mDelay(2);
      phost->device_prop.address = USBH_DEVICE_ADDRESS;
      
      /* user callback for device address assigned */
      phost->usr_cb->DeviceAddressAssigned();
      phost->EnumState = ENUM_GET_CFG_DESC;
      
      /* modify control channels to update device address */
      USBH_Modify_Channel (pdev,
                           phost->Control.hc_num_in,
                           phost->device_prop.address,
                           0,
                           0,
                           0);
      
      USBH_Modify_Channel (pdev,
                           phost->Control.hc_num_out,
                           phost->device_prop.address,
                           0,
                           0,
                           0);         
    }
    break;
    
  case ENUM_GET_CFG_DESC:  
    /* get standard configuration descriptor */
    if ( USBH_Get_CfgDesc(pdev, 
                          phost,
                          USB_CONFIGURATION_DESC_SIZE) == USBH_OK)
    {
      phost->EnumState = ENUM_GET_FULL_CFG_DESC;
    }
    break;
    
  case ENUM_GET_FULL_CFG_DESC:  
    /* get FULL config descriptor (config, interface, endpoints) */
    if (USBH_Get_CfgDesc(pdev, 
                         phost,
                         phost->device_prop.Cfg_Desc.wTotalLength) == USBH_OK)
    {
      /* User callback for configuration descriptors available */
      phost->usr_cb->ConfigurationDescAvailable(&phost->device_prop.Cfg_Desc,
                                                      phost->device_prop.Itf_Desc,
                                                      phost->device_prop.Ep_Desc[0]);
      
      phost->EnumState = ENUM_GET_MFC_STRING_DESC;
    }
    break;
    
  case ENUM_GET_MFC_STRING_DESC:  
    if (phost->device_prop.Dev_Desc.iManufacturer != 0)
    { /* Check that Manufacturer String is available */
      
      if ( USBH_Get_StringDesc(pdev,
                               phost,
                               phost->device_prop.Dev_Desc.iManufacturer, 
                               Local_Buffer , 
                               0xff) == USBH_OK)
      {
        /* User callback for Manufacturing string */
        phost->usr_cb->ManufacturerString(Local_Buffer);
        phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;
      }
    }
    else
    {
      phost->usr_cb->ManufacturerString("N/A");      
      phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;
    }
    break;
    
  case ENUM_GET_PRODUCT_STRING_DESC:   
    if (phost->device_prop.Dev_Desc.iProduct != 0)
    { /* Check that Product string is available */
      if ( USBH_Get_StringDesc(pdev,
                               phost,
                               phost->device_prop.Dev_Desc.iProduct, 
                               Local_Buffer, 
                               0xff) == USBH_OK)
      {
        /* User callback for Product string */
        phost->usr_cb->ProductString(Local_Buffer);
        phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;
      }
    }
    else
    {
      phost->usr_cb->ProductString("N/A");
      phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;
    } 
    break;
    
  case ENUM_GET_SERIALNUM_STRING_DESC:   
    if (phost->device_prop.Dev_Desc.iSerialNumber != 0)
    { /* Check that Serial number string is available */    
      if ( USBH_Get_StringDesc(pdev, 
                               phost,
                               phost->device_prop.Dev_Desc.iSerialNumber, 
                               Local_Buffer, 
                               0xff) == USBH_OK)
      {
        /* User callback for Serial number string */
        phost->usr_cb->SerialNumString(Local_Buffer);
        phost->EnumState = ENUM_SET_CONFIGURATION;
      }
    }
    else
    {
      phost->usr_cb->SerialNumString("N/A");      
      phost->EnumState = ENUM_SET_CONFIGURATION;
    }  
    break;
      
  case ENUM_SET_CONFIGURATION:
    /* set configuration  (default config) */
    if (USBH_SetCfg(pdev, 
                    phost,
                    phost->device_prop.Cfg_Desc.bConfigurationValue) == USBH_OK)
    {
      phost->EnumState = ENUM_DEV_CONFIGURED;
    }
    break;

    
  case ENUM_DEV_CONFIGURED:
    /* user callback for enumeration done */
    Status = USBH_OK;
    break;
    
  default:
    break;
  }  
  return Status;
}


/**
  * @brief  USBH_HandleControl
  *         Handles the USB control transfer state machine
  * @param  pdev: Selected device
  * @retval Status
  USB控制传输状态机处理函数
  */
USBH_Status USBH_HandleControl (USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  uint8_t direction;  
  static uint16_t timeout = 0;
  USBH_Status status = USBH_OK;
  URB_STATE URB_Status = URB_IDLE;
  
  phost->Control.status = CTRL_START;

  
  switch (phost->Control.state)
  {
  case CTRL_SETUP:   //控制建立阶段 发送8个字节的控制包
    /* send a SETUP packet */
    USBH_CtlSendSetup     (pdev, 
	                   phost->Control.setup.d8 , 
	                   phost->Control.hc_num_out);  
    phost->Control.state = CTRL_SETUP_WAIT;  
    break; 
    
  case CTRL_SETUP_WAIT://控制setup包发送后等待数据传输完成
    
    URB_Status = HCD_GetURB_State(pdev , phost->Control.hc_num_out); //等待数据传输完成
    /* case SETUP packet sent successfully */
    if(URB_Status == URB_DONE)//数据传输完成
    { 
      direction = (phost->Control.setup.b.bmRequestType & USB_REQ_DIR_MASK);
      
      /* check if there is a data stage */
      if (phost->Control.setup.b.wLength.w != 0 )
      {        
        timeout = DATA_STAGE_TIMEOUT;
        if (direction == USB_D2H)//设备到主机
        {
          /* Data Direction is IN */
          phost->Control.state = CTRL_DATA_IN;
        }
        else
        {
          /* Data Direction is OUT */
          phost->Control.state = CTRL_DATA_OUT;
        } 
      }
      /* No DATA stage 没有数据内容的控制包*/
      else
      {
        timeout = NODATA_STAGE_TIMEOUT;
        
        /* If there is No Data Transfer Stage */
        if (direction == USB_D2H)
        {
          /* Data Direction is IN */
          phost->Control.state = CTRL_STATUS_OUT;
        }
        else
        {
          /* Data Direction is OUT */
          phost->Control.state = CTRL_STATUS_IN;
        } 
      }          
      /* Set the delay timer to enable timeout for data stage completion */
      phost->Control.timer = HCD_GetCurrentFrame(pdev);
    }
    else if(URB_Status == URB_ERROR)
    {
      phost->Control.state = CTRL_ERROR;     
      phost->Control.status = CTRL_XACTERR;
    }    
    break;
    
  case CTRL_DATA_IN:   //控制传输完成 数据输入阶段
    /* Issue an IN token */ 
    USBH_CtlReceiveData(pdev,
                        phost->Control.buff, 
                        phost->Control.length,
                        phost->Control.hc_num_in);
 
    phost->Control.state = CTRL_DATA_IN_WAIT;
    break;    
    
  case CTRL_DATA_IN_WAIT:
    
    URB_Status = HCD_GetURB_State(pdev , phost->Control.hc_num_in); 
    
    /* check is DATA packet transfered successfully */
    if  (URB_Status == URB_DONE)
    { 
      phost->Control.state = CTRL_STATUS_OUT;
    }
   
    /* manage error cases*/
    if  (URB_Status == URB_STALL) 
    { 
      /* In stall case, return to previous machine state*/
      phost->gState =   phost->gStateBkp;
    }   
    else if (URB_Status == URB_ERROR)
    {
      /* Device error */
      phost->Control.state = CTRL_ERROR;    
    }
    else if ((HCD_GetCurrentFrame(pdev)- phost->Control.timer) > timeout)//超时
    {
      /* timeout for IN transfer */
      phost->Control.state = CTRL_ERROR; 
    }   
    break;
    
  case CTRL_DATA_OUT:
    /* Start DATA out transfer (only one DATA packet)*/
    pdev->host.hc[phost->Control.hc_num_out].toggle_out = 1; 
        
    USBH_CtlSendData (pdev,
                      phost->Control.buff, 
                      phost->Control.length , 
                      phost->Control.hc_num_out);
    



    
    phost->Control.state = CTRL_DATA_OUT_WAIT;
    break;
    
  case CTRL_DATA_OUT_WAIT:
    
    URB_Status = HCD_GetURB_State(pdev , phost->Control.hc_num_out);     
    if  (URB_Status == URB_DONE)
    { /* If the Setup Pkt is sent successful, then change the state */
      phost->Control.state = CTRL_STATUS_IN;
    }
    
    /* handle error cases */
    else if  (URB_Status == URB_STALL) 
    { 
      /* In stall case, return to previous machine state*/
      phost->gState =   phost->gStateBkp;
      phost->Control.state = CTRL_STALLED;  
    } 
    else if  (URB_Status == URB_NOTREADY)
    { 
      /* Nack received from device */
      phost->Control.state = CTRL_DATA_OUT;
    }    
    else if (URB_Status == URB_ERROR)
    {
      /* device error */
      phost->Control.state = CTRL_ERROR;      
    } 
    break;
    
    
  case CTRL_STATUS_IN:
    /* Send 0 bytes out packet */
    USBH_CtlReceiveData (pdev,
                         0,
                         0,
                         phost->Control.hc_num_in);
    
    phost->Control.state = CTRL_STATUS_IN_WAIT;
    
    break;
    
  case CTRL_STATUS_IN_WAIT:
    
    URB_Status = HCD_GetURB_State(pdev , phost->Control.hc_num_in); 
    
    if  ( URB_Status == URB_DONE)
    { /* Control transfers completed, Exit the State Machine */
      phost->gState =   phost->gStateBkp;
      phost->Control.state = CTRL_COMPLETE;
    }
    
    else if (URB_Status == URB_ERROR)
    {
      phost->Control.state = CTRL_ERROR;  
    }
    
    else if((HCD_GetCurrentFrame(pdev)\
      - phost->Control.timer) > timeout)
    {
      phost->Control.state = CTRL_ERROR; 
    }
     else if(URB_Status == URB_STALL)
    {
      /* Control transfers completed, Exit the State Machine */
      phost->gState =   phost->gStateBkp;
      phost->Control.status = CTRL_STALL;
      status = USBH_NOT_SUPPORTED;
    }
    break;
    
  case CTRL_STATUS_OUT://发送控制数据
    pdev->host.hc[phost->Control.hc_num_out].toggle_out ^= 1; 
    USBH_CtlSendData (pdev,
                      0,
                      0,
                      phost->Control.hc_num_out);
    
    phost->Control.state = CTRL_STATUS_OUT_WAIT;
    break;
    
  case CTRL_STATUS_OUT_WAIT: 
    
    URB_Status = HCD_GetURB_State(pdev , phost->Control.hc_num_out);  
    if  (URB_Status == URB_DONE)
    { 
      phost->gState =   phost->gStateBkp; 
      phost->Control.state = CTRL_COMPLETE; 
    }
    else if  (URB_Status == URB_NOTREADY)
    { 
      phost->Control.state = CTRL_STATUS_OUT;
    }      
    else if (URB_Status == URB_ERROR)
    {
      phost->Control.state = CTRL_ERROR;      
    }
    break;
    
  case CTRL_ERROR:
    /* 
    After a halt condition is encountered or an error is detected by the 
    host, a control endpoint is allowed to recover by accepting the next Setup 
    PID; i.e., recovery actions via some other pipe are not required for control
    endpoints. For the Default Control Pipe, a device reset will ultimately be 
    required to clear the halt or error condition if the next Setup PID is not 
    accepted.
    */
    if (++ phost->Control.errorcount <= USBH_MAX_ERROR_COUNT)
    {
      /* Do the transmission again, starting from SETUP Packet */
      phost->Control.state = CTRL_SETUP; 
    }
    else
    {
      phost->Control.status = CTRL_FAIL;
      phost->gState =   phost->gStateBkp;
      
      status = USBH_FAIL;
    }
    break;
    
  default:
    break;
  }
  return status;
}


/**
* @}
*/ 

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





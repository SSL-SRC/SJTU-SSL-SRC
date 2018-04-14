/**
  ******************************************************************************
  * @file    usbh_def.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   Definitions used in the USB host library
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

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_DEF
  * @brief This file is includes USB descriptors
  * @{
  */ 

#ifndef  USBH_DEF_H
#define  USBH_DEF_H

#ifndef USBH_NULL
#define USBH_NULL ((void *)0)
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#define ValBit(VAR,POS)                               (VAR & (1 << POS))
#define SetBit(VAR,POS)                               (VAR |= (1 << POS))
#define ClrBit(VAR,POS)                               (VAR &= ((1 << POS)^255))

#define  LE16(addr)             (((u16)(*((u8 *)(addr))))\
                                + (((u16)(*(((u8 *)(addr)) + 1))) << 8))

#define  USB_LEN_DESC_HDR                               0x02
#define  USB_LEN_DEV_DESC                               0x12
#define  USB_LEN_CFG_DESC                               0x09
#define  USB_LEN_IF_DESC                                0x09
#define  USB_LEN_EP_DESC                                0x07
#define  USB_LEN_OTG_DESC                               0x03
#define  USB_LEN_SETUP_PKT                              0x08

/* bmRequestType :D7 Data Phase Transfer Direction  Setup packet 详细见usb2.0协议文档第9章*/
#define  USB_REQ_DIR_MASK                               0x80
#define  USB_H2D                                        0x00   //host to device
#define  USB_D2H                                        0x80

/* bmRequestType D6..5 Type */
#define  USB_REQ_TYPE_STANDARD                          0x00   //usb2.0 9.3节 table 9-2
#define  USB_REQ_TYPE_CLASS                             0x20
#define  USB_REQ_TYPE_VENDOR                            0x40
#define  USB_REQ_TYPE_RESERVED                          0x60

/* bmRequestType D4..0 Recipient */
#define  USB_REQ_RECIPIENT_DEVICE                       0x00
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02
#define  USB_REQ_RECIPIENT_OTHER                        0x03

/* Table 9-4. Standard Request Codes  */
/* bRequest , Value */
#define  USB_REQ_GET_STATUS                             0x00
#define  USB_REQ_CLEAR_FEATURE                          0x01
#define  USB_REQ_SET_FEATURE                            0x03
#define  USB_REQ_SET_ADDRESS                            0x05
#define  USB_REQ_GET_DESCRIPTOR                         0x06
#define  USB_REQ_SET_DESCRIPTOR                         0x07
#define  USB_REQ_GET_CONFIGURATION                      0x08
#define  USB_REQ_SET_CONFIGURATION                      0x09
#define  USB_REQ_GET_INTERFACE                          0x0A
#define  USB_REQ_SET_INTERFACE                          0x0B
#define  USB_REQ_SYNCH_FRAME                            0x0C

/* Table 9-5. Descriptor Types of USB Specifications */
#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_INTERFACE_POWER                     8
#define  USB_DESC_TYPE_HID                                 0x21
#define  USB_DESC_TYPE_HID_REPORT                          0x22


#define USB_DEVICE_DESC_SIZE                               18   //设备描述符大小
#define USB_CONFIGURATION_DESC_SIZE                        9
#define USB_HID_DESC_SIZE                                  9
#define USB_INTERFACE_DESC_SIZE                            9
#define USB_ENDPOINT_DESC_SIZE                             7

/* Descriptor Type and Descriptor Index  */
/* Use the following values when calling the function USBH_GetDescriptor  */
#define  USB_DESC_DEVICE                    ((USB_DESC_TYPE_DEVICE << 8) & 0xFF00)
#define  USB_DESC_CONFIGURATION             ((USB_DESC_TYPE_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_STRING                    ((USB_DESC_TYPE_STRING << 8) & 0xFF00)
#define  USB_DESC_INTERFACE                 ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_ENDPOINT                  ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_DEVICE_QUALIFIER          ((USB_DESC_TYPE_DEVICE_QUALIFIER << 8) & 0xFF00)
#define  USB_DESC_OTHER_SPEED_CONFIGURATION ((USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_INTERFACE_POWER           ((USB_DESC_TYPE_INTERFACE_POWER << 8) & 0xFF00)
#define  USB_DESC_HID_REPORT                ((USB_DESC_TYPE_HID_REPORT << 8) & 0xFF00)
#define  USB_DESC_HID                       ((USB_DESC_TYPE_HID << 8) & 0xFF00)


#define  USB_EP_TYPE_CTRL                               0x00
#define  USB_EP_TYPE_ISOC                               0x01
#define  USB_EP_TYPE_BULK                               0x02
#define  USB_EP_TYPE_INTR                               0x03

#define  USB_EP_DIR_OUT                                 0x00
#define  USB_EP_DIR_IN                                  0x80
#define  USB_EP_DIR_MSK                                 0x80  

/* supported classes */
#define USB_MSC_CLASS                                   0x08
#define USB_HID_CLASS                                   0x03

/* Interface Descriptor field values for HID Boot Protocol */
#define HID_BOOT_CODE                                  0x01    
#define HID_KEYBRD_BOOT_CODE                           0x01
#define HID_MOUSE_BOOT_CODE                            0x02
/***北通无线游戏手柄***/
#define HID_JOYSTICK_GAME_BOOT_CODE                    0x03  	//
#define HID_JOYSTICK_SUB_CLASS_CODE                    0x0//0x5D
#define HID_JOYSTICK_PROCOTOL_CODE                    0x0// 0x03

/* As per USB specs 9.2.6.4 :Standard request with data request timeout: 5sec
   Standard request with no data stage timeout : 50ms */
#define DATA_STAGE_TIMEOUT                              5000 
#define NODATA_STAGE_TIMEOUT                            50

/**
  * @}
  */ 


#define USBH_CONFIGURATION_DESCRIPTOR_SIZE (USB_CONFIGURATION_DESC_SIZE \
                                           + USB_INTERFACE_DESC_SIZE\
                                           + (USBH_MAX_NUM_ENDPOINTS * USB_ENDPOINT_DESC_SIZE))


#define CONFIG_DESC_wTOTAL_LENGTH (ConfigurationDescriptorData.ConfigDescfield.\
                                          ConfigurationDescriptor.wTotalLength)


/*  This Union is copied from usb_core.h  */
typedef union
{
  uint16_t w;
  struct BW
  {
    uint8_t msb;
    uint8_t lsb;
  }
  bw;
}
uint16_t_uint8_t;

/***setup packet 结构体定义 详见usb2.0协议   控制传输结构体用于枚举设备***/
typedef union _USB_Setup
{
  uint8_t d8[8];
  
  struct _SetupPkt_Struc
  {
    uint8_t           bmRequestType;  //D7 1 设备到主机 0 主机到设备 D6..5  0 standard 1 calss 2 vendor 3 resverved
    uint8_t           bRequest;       //Table 9-3
    uint16_t_uint8_t  wValue;
    uint16_t_uint8_t  wIndex;
    uint16_t_uint8_t  wLength;
  } b;
} 
USB_Setup_TypeDef;  

typedef  struct  _DescHeader 
{
    uint8_t  bLength;       
    uint8_t  bDescriptorType;
} 
USBH_DescHeader_t;

typedef struct _DeviceDescriptor
{
  uint8_t   bLength;                 //设备描述符的字节数
  uint8_t   bDescriptorType;         //设备描述符类型编号
  uint16_t  bcdUSB;        /* USB Specification Number which device complies too 0x0200 USB2.0 USB版本号*/
  uint8_t   bDeviceClass;            //USB分配的设备类
  uint8_t   bDeviceSubClass;         //USB分配的设备子类
  uint8_t   bDeviceProtocol;         //USB分配的设备协议代码
  /* If equal to Zero, each interface specifies its own class
  code if equal to 0xFF, the class code is vendor specified.
  Otherwise field is valid Class Code.*/
  uint8_t   bMaxPacketSize;   //设备描述符的最大包大小是指控制端点0的最大包大小
  uint16_t  idVendor;      /* Vendor ID (Assigned by USB Org) 厂商编号*/
  uint16_t  idProduct;     /* Product ID (Assigned by Manufacturer) 产品编号 */
  uint16_t  bcdDevice;     /* Device Release Number 设备出厂编号*/
  uint8_t   iManufacturer;  /* Index of Manufacturer String Descriptor 设备厂商字符串索引*/
  uint8_t   iProduct;       /* Index of Product String Descriptor  产品字符串索引*/
  uint8_t   iSerialNumber;  /* Index of Serial Number String Descriptor 设备序列号索引*/
  uint8_t   bNumConfigurations; /* Number of Possible Configurations 可能的配置参数*/
}
USBH_DevDesc_TypeDef;


typedef struct _ConfigurationDescriptor
{
  uint8_t   bLength;                         //配置描述符的字节数
  uint8_t   bDescriptorType;            //配置描述符的类型编号
  uint16_t  wTotalLength;        /* Total Length of Data Returned 此配置返回的所有数据大小*/
  uint8_t   bNumInterfaces;       /* Number of Interfaces 此配置支持的接口数量*/
  uint8_t   bConfigurationValue;  /* Value to use as an argument to select this configuration*/
  uint8_t   iConfiguration;       /*Index of String Descriptor Describing this configuration 描述该配置的字符串索引*/
  uint8_t   bmAttributes;         /* D7 Bus Powered , D6 Self Powered, D5 Remote Wakeup , D4..0 Reserved (0) 供电模式的选择*/
  uint8_t   bMaxPower;            /*Maximum Power Consumption 设备从总线获取的最大电流*/
}
USBH_CfgDesc_TypeDef;


typedef struct _HIDDescriptor
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdHID;               /* indicates what endpoint this descriptor is describing */
  uint8_t   bCountryCode;        /* specifies the transfer type. */
  uint8_t   bNumDescriptors;     /* specifies the transfer type. */
  uint8_t   bReportDescriptorType;    /* Maximum Packet Size this endpoint is capable of sending or receiving */  
  uint16_t  wItemLength;          /* is used to specify the polling interval of certain transfers. */
}
USBH_HIDDesc_TypeDef;


typedef struct _InterfaceDescriptor
{
  uint8_t bLength;                //接口描述符的字节数 结构体长度 9
  uint8_t bDescriptorType;        //接口描述符的类型编号
  uint8_t bInterfaceNumber;       //该接口的编号
  uint8_t bAlternateSetting;    /* Value used to select alternative setting 备用的接口描述符编号*/
  uint8_t bNumEndpoints;        /* Number of Endpoints used for this interface 该接口使用的端点数 不包括端点0*/
  uint8_t bInterfaceClass;      /* Class Code 0x03 表示HID设备(Assigned by USB Org) 接口类*/
  uint8_t bInterfaceSubClass;   /* Subclass Code (Assigned by USB Org)  0X01 boot interface 接口子类*/
  uint8_t bInterfaceProtocol;   /* Protocol Code  0x02 mouse  0x01 keyboard接口类协议*/
  uint8_t iInterface;           /* Index of String Descriptor Describing this interface 描述该接口的字符串索引 */
  
}
USBH_InterfaceDesc_TypeDef;


typedef struct _EndpointDescriptor
{
  uint8_t   bLength;               //端点描述符字节数 结构体长度 7
  uint8_t   bDescriptorType;       //端点描述符类型编号
  uint8_t   bEndpointAddress;   /* indicates what endpoint this descriptor is describing  0x81 IN  端点地址 输入输出类型*/
  uint8_t   bmAttributes;       /* specifies the transfer type. 0x03 INTERRUPT  端点传输类型*/
  uint16_t  wMaxPacketSize;    /* Maximum Packet Size this endpoint is capable of sending or receiving 端点收发的最大包大小*/  
  uint8_t   bInterval;          /* is used to specify the polling interval of certain transfers. 主机查询端点的时间间隔*/
}
USBH_EpDesc_TypeDef;
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


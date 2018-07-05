#include "Usbh_hid_joystick.h"
#include "string.h"
#include "gpio.h"
static void  JOYSTICK_Init (void);
static void  JOYSTICK_Decode(uint8_t *data);
/**
  * @}
  */ 


/** @defgroup USBH_HID_MOUSE_Private_Variables
  * @{
  */
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
 #if defined   (__CC_ARM) /*!< ARM Compiler */
  __align(4) 
 #elif defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
 #elif defined (__GNUC__) /*!< GNU Compiler */
 #pragma pack(4) 
 #elif defined  (__TASKING__) /*!< TASKING Compiler */                           
  __align(4) 
 #endif /* __CC_ARM */
#endif
 
 
HID_JOYSTICK_Data_TypeDef HID_JOYSTICK_Data = {0};
HID_cb_TypeDef HID_JOYSTICK_cb = 
{
  JOYSTICK_Init,              //游戏手柄初始化
  JOYSTICK_Decode,
};



static void  JOYSTICK_Init ( void)
{
  
  memset(&HID_JOYSTICK_Data,0,sizeof(HID_JOYSTICK_Data));
}
            

static void  JOYSTICK_Decode(uint8_t *data)
{
  
    memcpy(&HID_JOYSTICK_Data.key_button,data,sizeof(HID_JOYSTICK_Data));
	if((HID_JOYSTICK_Data.key_button.bit.Betop_A_Key == 1))
	{
	//   COMM_LED_ON();
	}
	else
	{
	 //  COMM_LED_OFF();
	}
	HID_JOYSTICK_Data.packet_valid = 1;
	
}





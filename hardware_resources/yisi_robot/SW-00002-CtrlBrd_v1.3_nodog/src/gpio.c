 /********************************************************************************
 ********************************************************************************
 * 权限:	杭州南江机器人股份有限公司
 * 文件名:	  gpio.c 		
 * 功能描述:	
		  gpio相关初始化
 * 版本 	 作者		时间		  状态
 * V1.0 	 shouxian	2015.9.10	创建文件
 * V1.1      sunlifeng  2016.2.28   修改文件
 *****************************************************************************
 *****************************************************************************/
#include "gpio.h"

#define IS_ENABLE_WTD 1
/*******************************************************************************
* Function Name   : Init_wdg
* Description	  : 看门狗初始化
*					
* Input		      : u8  prer,u16 rlr
* Output		  : None
* Return		  : None
*******************************************************************************/
void Init_wdg(u8  prer,u16 rlr)
{
	/* LSI Enable */
	RCC_LSICmd(ENABLE);
	
	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) ;

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //LSI 32khz

	/* IWDG counter clock: LSI */
	//IWDG_SetPrescaler(IWDG_Prescaler_32);  32K/64=0.5K
	IWDG_SetPrescaler(prer);

     IWDG_SetReload(rlr);
	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
#if IS_ENABLE_WTD
	IWDG_Enable();
#endif
}

/*******************************************************************************
* Function Name   : feed_iwdog
* Description	  : 喂狗
*					
* Input		      : None 
* Output		  : None
* Return		  : None.
*******************************************************************************/
void feed_iwdog(void)
{    
  #if IS_ENABLE_WTD  
	 IWDG_ReloadCounter();
  #endif
}
/*******************************************************************************
* Function Name   : init_gpio
* Description	  : gpio相关函数初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_gpio(void)
{
	COMM_LED_OFF();
	LED_POWER_ON_LOW();
	RGB_LED_CLK_HI();
	RGB_LED_DAT_LOW();
	BTE_NRST_HI();
	BEEP_OFF();
	DIP_SW_CLK_LOW();
	DIP_SW_LD_LOW();
	NRF_CE_LOW();
	NRF_CE_HIGH();
	USB_V_EN_HI();
}



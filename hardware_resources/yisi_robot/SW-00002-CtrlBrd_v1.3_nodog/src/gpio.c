 /********************************************************************************
 ********************************************************************************
 * Ȩ��:	�����Ͻ������˹ɷ����޹�˾
 * �ļ���:	  gpio.c 		
 * ��������:	
		  gpio��س�ʼ��
 * �汾 	 ����		ʱ��		  ״̬
 * V1.0 	 shouxian	2015.9.10	�����ļ�
 * V1.1      sunlifeng  2016.2.28   �޸��ļ�
 *****************************************************************************
 *****************************************************************************/
#include "gpio.h"

#define IS_ENABLE_WTD 1
/*******************************************************************************
* Function Name   : Init_wdg
* Description	  : ���Ź���ʼ��
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
* Description	  : ι��
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
* Description	  : gpio��غ�����ʼ��
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



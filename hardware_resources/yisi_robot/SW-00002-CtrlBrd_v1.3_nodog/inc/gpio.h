 /********************************************************************************
 ********************************************************************************
 * 权限:	杭州南江机器人股份有限公司
 * 文件名:	  gpio.h		
 * 功能描述:	
		  gpio相关宏定义
 * 版本 	 作者		时间		  状态
 * V1.0 	 shouxian	2015.9.10	创建文件
 * V1.1      sunlifeng  2016.2.28   修改文件
 *****************************************************************************
 *****************************************************************************/
#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"

/* define gpio macro */
//#define GET_CAP_LOW_IO()	((GPIOC->IDR & GPIO_Pin_13) == GPIO_Pin_13)
#define GET_CAP_LOW_IO()	1

#define GET_BALL_DECET_IO()	((GPIOC->IDR & GPIO_Pin_0) == GPIO_Pin_0)

#define COMM_LED_ON()		GPIOA->BSRRL = GPIO_Pin_8
#define COMM_LED_OFF()		GPIOA->BSRRH = GPIO_Pin_8

#define BEEP_ON()			GPIOC->BSRRL = GPIO_Pin_1
#define BEEP_OFF()			GPIOC->BSRRH = GPIO_Pin_1

#define GET_NRF_IRQ_IO()	((GPIOD->IDR & GPIO_Pin_0) == GPIO_Pin_0)

#define NRF_CE_LOW()		GPIOD->BSRRH = GPIO_Pin_2
#define NRF_CE_HIGH()		GPIOD->BSRRL = GPIO_Pin_2

#define NRF_NCS_LOW()		GPIOD->BSRRH = GPIO_Pin_1
#define NRF_NCS_HIGH()		GPIOD->BSRRL = GPIO_Pin_1

#define DIP_SW_CLK_LOW()	GPIOC->BSRRH = GPIO_Pin_8	
#define DIP_SW_CLK_HI()		GPIOC->BSRRL = GPIO_Pin_8	

#define DIP_SW_LD_LOW()		GPIOC->BSRRH = GPIO_Pin_7
#define DIP_SW_LD_HI()		GPIOC->BSRRL = GPIO_Pin_7

#define GET_DIP_SW_D_IO()	((GPIOC->IDR & GPIO_Pin_9) == GPIO_Pin_9)

#define MOTOR_BALL_DIR_LOW()		GPIOE->BSRRH = GPIO_Pin_10
#define MOTOR_BALL_DIR_HI()			GPIOE->BSRRL = GPIO_Pin_10	

#define MOTOR_BALL_BRK_LOW()		GPIOE->BSRRH = GPIO_Pin_8
#define MOTOR_BALL_BRK_HI()			GPIOE->BSRRL = GPIO_Pin_8	

#define RGB_LED_CLK_LOW()	GPIOA->BSRRH = GPIO_Pin_5
#define RGB_LED_CLK_HI()	GPIOA->BSRRL = GPIO_Pin_5

#define RGB_LED_DAT_LOW()	GPIOB->BSRRH = GPIO_Pin_5
#define RGB_LED_DAT_HI()	GPIOB->BSRRL = GPIO_Pin_5

#define BTE_NRST_LOW()		GPIOB->BSRRH = GPIO_Pin_14
#define BTE_NRST_HI()		GPIOB->BSRRL = GPIO_Pin_14

#define USB_V_EN_LOW()		GPIOD->BSRRH = GPIO_Pin_3
#define USB_V_EN_HI()		GPIOD->BSRRL = GPIO_Pin_3

#define LED_POWER_ON_HIGH()		GPIOD->BSRRL = GPIO_Pin_10
#define LED_POWER_ON_LOW()		GPIOD->BSRRH = GPIO_Pin_10


void init_gpio(void);
void Init_wdg(u8  prer,u16 rlr);
void feed_iwdog(void);

#endif
 

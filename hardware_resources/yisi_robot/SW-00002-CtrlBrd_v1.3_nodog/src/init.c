/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    Init.c          
* 功能描述:    
           硬件相关初始化
* 版本      作者       时间          状态
* V1.0      chensx     2015.08.08    创建文件
* V1.1      sunlf      2016.03.29    修改文件
*****************************************************************************
*****************************************************************************/ 
#include "init.h"
#include "timer.h"
#include "motor.h"
#include "gpio.h"
#include "i2c.h"
#include "robot.h"
#include "core_cm4.h"
#include "misc.h"
#include "comm.h"
#include "usart_ble.h"
#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "usbd_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "MPU6050_driver.h"
#include <stm32f4xx.h>
#include "error_event.h"
#include "Cpuid_des.h"

u32 cpuID[3];  //32bit cpuid
u8  cpuid_data[12]; //12Byte couid
u8  encrpty_cpuid[8];//加密后的cpuid
char usb_host_device_mode = 0;
char eeprom_blue_init_flag = 0; //存放于eeprom中的标志用于告知蓝牙模块是否已经初始化 只初始化一次
extern int Init_hm_blue_module(void);

/*******************************************************************************
* Function Name   : setup_clock
* Description	  : 设置控制器板卡时钟
*
* Input 		  : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void setup_clock (void)
{

	/*!< At this stage the microcontroller clock setting is already configured,
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f4xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f4xx.c file
	 */

	/* at 25MHz input clock, the clock of system is as below:
		HCLK(SysCLK) : 	168MHz
		PCLK1(APB_1) : 	42MHz
		PCLK2(APB_2) : 	84MHz
	*/

	/* GPIO Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
		RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* TIM5 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* TIM6 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* TIM7 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	/* TIM9 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

	/* TIM10 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

	/* TIM11 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
    /* TIM12 clock enable for usb timedelay*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

	/* SPI 1 clock enable. for heart LED */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* SPI 3 clock enable. for 2.4G RF */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	/* UART 3 clock enable. for BTE */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* ADC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

}


/*******************************************************************************s
* Function Name   : setup_NVIC
* Description	  : 设置中断使能优先级
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void setup_NVIC(void)
{
	  NVIC_InitTypeDef NVIC_InitStructure;
	  init_ball_dect_irq();

	 /****配置2位抢占式优先级和2位子优先级****/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//蓝牙串口模块通信
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	
}

/*******************************************************************************
* Function Name   : setup_GPIO
* Description	  : 硬件板卡GPIO初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void setup_GPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* encoder input, tim2 to tim5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5); //PA0 PA1复用功能TIM5 编码器捕获

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3); //PA6 PA7复用功能TIM3

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4); //PB6 PB7复用功能TIM4

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2); 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2); //PA15 PB3复用功能TIM2

	/* enable motor 1 to 4 PWM, TIM 1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);

	/* enable ball motor PWM, TIM 11 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM11);

	/* enable IR PWM, TIM 10 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);

	/* enable SHOOT & CHIP PWM, TIM 9 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9);

	/* enable Heart LED, SPI 1 */
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	*/

	/* ADC analog input, PB0 & PB1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* USB */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_OTG_FS);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG_FS);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG_FS);

	/* 2.4G RF SPI 3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

	/* BTE UART 3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_USART3);

	/* PA port input, PA9, USB_VBUS */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA9 PA10 usart1 */
	/*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	*/

	/* PA port output, PA8, LED D4 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* I2C pin, PB10, PB11 gpio模拟I2C时序*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PB, BTE_rst, PB14, */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PC, input pin, */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14
		| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PC, output pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PD, input pin, */
	/* PD0, 2.4G RF IRQ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* PD, output pin  PD10 for LED_POWER_ON*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* PE, output pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
		| GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* enable Heart LED, IO port */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name   : sys_tick_init
* Description	  : 系统时钟初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void sys_tick_init(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	sys_tick = 0;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / SYS_TICK_FREQ);
}

/*******************************************************************************
* Function Name   : read_cpu_id
* Description	  : 读取cpu id序列号
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void read_cpu_id(void)
{
    int i,j,k;
	  u8 cpuid_l[4];
	  k = 0;
	  cpuID[0] = *(volatile u32*)(0x1FFF7A10);
    cpuID[1] = *(volatile u32*)(0x1FFF7A14);
    cpuID[2] = *(volatile u32*)(0x1FFF7A18);
    for(i = 0; i < 3 ; i ++)
    {
        cpuid_l[0] =  cpuID[i] & 0xff;
	      cpuid_l[1] = (cpuID[i] >> 8)& 0xff;
	      cpuid_l[2] = (cpuID[i] >> 16)& 0xff;
	      cpuid_l[3] = (cpuID[i] >> 24)& 0xff;
        for(j = 0; j < 4 ; j++)
        {
          cpuid_data[k++] = cpuid_l[j];
        } 
        
    }
	 
}

/*******************************************************************************
* Function Name   : calcute_encrpty_cpuid
* Description	  : cpuid 加密计算
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void calcute_encrpty_cpuid(void)
{
   
	encrypt_CPUID(&encrpty_cpuid[0], &cpuid_data[0], 12);
}

/*******************************************************************************
* Function Name   : LOCK_FLASH_RDP
* Description	  : flash读保护设置,防止通过调试工具读出二进制代码
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void LOCK_FLASH_RDP(void)
{

	if(SET != FLASH_OB_GetRDP())//获取RDP设置状态 已经设置不用再次设置
    {
        
        FLASH_Unlock();
        FLASH_OB_Unlock();  //解除OPTCR选项寄存器
        FLASH_OB_RDPConfig(OB_RDP_Level_1);//设置1级读保护
        FLASH_OB_Launch();  //触发用户选项操作
        FLASH_OB_Lock();
        FLASH_Lock();
    }
}

/*******************************************************************************
* Function Name   : UNLOCK_FLASH_RDP
* Description	  : 解锁falsh读保护 会擦除flash程序 谨慎使用
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void UNLOCK_FLASH_RDP(void)
{

	if(SET == FLASH_OB_GetRDP())//获取RDP设置状态 已经设置读保护 执行
    {
        
        FLASH_Unlock();
        FLASH_OB_Unlock();  //解除OPTCR选项寄存器
        FLASH_OB_RDPConfig(OB_RDP_Level_0);//设置0级读保护 解除读保护
        FLASH_OB_Launch();  //触发用户选项操作
        FLASH_OB_Lock();
        FLASH_Lock();
    }
}

/*******************************************************************************
* Function Name   : init_sys
* Description	  : 整个系统初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void init_sys(void)
{   
   
	int rtn = 0;
	int delay;
	
	//LOCK_FLASH_RDP();   //flash读保护 防止程序被读出
	init_gpio();
	init_wait_timer();
	init_shoot_chip_timer();
	init_motor();
	init_dribbler();
	init_i2c();
	init_nf24l01_spi();
	init_ir_pwm();
	init_adc();
	Init_com3(BaudRate_115200);
	for(delay = 0;delay < 50000000 ; delay++);
	#if MPU6050_GYRO_USED
    if(InitMPU6050() == 0) //陀螺仪初始化失败
    {  
     int i = 0;
     error_flag.bit.mpu6050_flag = 1;
	   BEEP_ON();
	   for(i = 0;i < 10000000 ; i++);
	   BEEP_OFF();
	   for(i = 0;i < 10000000 ; i++);
    }
	#endif
	EN_INT();

	init_robot();
	init_comm();
	//read_cpu_id();
	//calcute_encrpty_cpuid();
	Init_wdg(4,1000);  //2 s看门狗定时
	if(eeprom_blue_init_flag == 0)//蓝牙模块未初始化过
	{   
	   int i = 0;
	   rtn = Init_hm_blue_module();
	   if(rtn == 1)//蓝牙模块初始化成功 则向eeprom 参数data[15]置1表示已经初始化过
	   {
	       load_blue_param();
	   }
	   else
	   {
	     error_flag.bit.ble_hm13_flag = 1;
		   BEEP_ON();
	     for(i = 0;i < 10000000 ; i++);
	     BEEP_OFF();
	     for(i = 0;i < 10000000 ; i++);
	   }
	}
	
	switch(g_robot.mode)
	{
	  case JOYSTICK_MODE :  //USB主机模式 手柄模式
	  	   usb_host_device_mode = 0;
          USBH_Init(&USB_OTG_Core_dev, USB_OTG_FS_CORE_ID,&USB_Host,&HID_callb,&USR_Callbacks);
		  
		  #if MPU6050_GYRO_USED
		  
		  I2C_MPU6050_GPIO_Config();
		  I2C_MPU6050_Mode_Config();
		  Caulate_adc_offset();
		  
		  
		  #endif
		break;

	  case DEBUG_MODE :     //usb设备模式
	  	    usb_host_device_mode = 1;
          USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_CDC_cb,&USR_cb);
		break;

		default:
			break;
	 	
	}
   	
}

/*******************************************************************************
* Function Name   : do_init
* Description	  : 初始化硬件板卡
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_init(void)
{
	DIS_INT();
	
	/* System Clocks Configuration */
	setup_clock();

	/* NVIC Configuration */
	setup_NVIC();

	/* GPIO Configuration */
	setup_GPIO();

	/* system tick initial */
	sys_tick_init();

	/* initial system */
	init_sys();

	return 0;
}


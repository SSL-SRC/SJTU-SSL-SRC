/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    misc.c          
* 功能描述:    
           子功能函数
* 版本      作者       时间          状态
* V1.0      chensx     2015.09.10   创建文件 
*****************************************************************************
*****************************************************************************/ 
#include "misc.h"
#include "gpio.h"
#include "cfg.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "robot.h"
#include "timer.h"

extern vu8 is_action_shoot;


/*******************************************************************************
* Function Name   : heart_led_spi_wr
* Description	  : APA102七彩灯数据传输
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void heart_led_spi_wr(u8 val)
{
	u8 i;

	for(i = 0; i < 8; i++)
	{
		RGB_LED_CLK_LOW();
		
		if(val & 0x80) RGB_LED_DAT_HI();
		else RGB_LED_DAT_LOW();

		wait_us(1);	
		RGB_LED_CLK_HI();
		wait_us(1);	

		val = val << 1;
	}

	RGB_LED_CLK_HI();
}

/*******************************************************************************
* Function Name   : set_heart_led
* Description	  : APA102七彩灯颜色和亮度设置
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void set_heart_led(u8 r, u8 g, u8 b, u8 bright)
{
	u8 val;
	
	/* APA102 start frame, 4 byte 0 */
	heart_led_spi_wr(0x0);       
    heart_led_spi_wr(0x0);
    heart_led_spi_wr(0x0);
    heart_led_spi_wr(0x0);

	val = 0xe0 | (bright & 0x1f);
	heart_led_spi_wr(val);
	heart_led_spi_wr(b);
	heart_led_spi_wr(g);
	heart_led_spi_wr(r);
	
	/* APA102 end frame, 4 byte 0xff */
	heart_led_spi_wr(0xff);       
    heart_led_spi_wr(0xff);
    heart_led_spi_wr(0xff);
    heart_led_spi_wr(0xff);
}


/*******************************************************************************
* Function Name   : heart_beat
* Description	  : 
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void heart_beat(void)
{
	set_heart_led(100, 0, 0, 0x1f);
}

/*******************************************************************************
* Function Name   : shoot_off
* Description	  : shoot功能关闭
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void shoot_off(void)
{
	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);
	TIM_Cmd(TIM9, DISABLE);
}


/*******************************************************************************
* Function Name   : chip_off
* Description	  : chip功能关闭
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void chip_off(void)
{
	TIM_CCxCmd(TIM9, TIM_Channel_2, TIM_CCx_Disable);
	TIM_Cmd(TIM9, DISABLE);
}


/*******************************************************************************
* Function Name   : shoot_on
* Description	  : 开启shoot平射功能
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void shoot_on(u32 value)
{

	if(value > MAX_SHOT_STRENGTH) value = MAX_SHOT_STRENGTH;

	value = MAX_SHOT_STRENGTH + 3 - value;

	if(value == 0) value = 1;
	
	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxCmd(TIM9, TIM_Channel_2, TIM_CCx_Disable);
	TIM_SetCompare1(TIM9, value);
	TIM_Cmd(TIM9, ENABLE);
}


/*******************************************************************************
* Function Name   : chip_on
* Description	  : 开启chip挑射功能
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void chip_on(u32 value)
{
	if(value > MAX_SHOT_STRENGTH) value = MAX_SHOT_STRENGTH;

	value = MAX_SHOT_STRENGTH + 3 - value;

	if(value == 0) value = 1;

	TIM_CCxCmd(TIM9, TIM_Channel_2, TIM_CCx_Enable);
	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);
	
	TIM_SetCompare2(TIM9, value);
	TIM_Cmd(TIM9, ENABLE);
}

/*******************************************************************************
* Function Name   : init_shoot_chip_timer
* Description	  : 初始化平射挑射定时器
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_shoot_chip_timer(void)
{
	u16 prescale;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM9 is clocked by PCLK_2, prescale > 1, clk is PCLK_2*2 */
	prescale = (u16) (FREQ_PCLK_2 * 2 / SHOOT_TIM_CLK_FRQ) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = MAX_SHOT_STRENGTH + 2;
	TIM_TimeBaseStructure.TIM_Prescaler = prescale;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

	/* TIM9 PWM2 Mode configuration: ch1 & ch2, ch1 is shoot, and low is active
	 (there is a inveter gate on ch1 path), and ch2 is chip, hi active. */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);

	 /* One Pulse Mode selection */
  	TIM_SelectOnePulseMode(TIM9, TIM_OPMode_Single);
	 
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Disable);

	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);
	TIM_CCxCmd(TIM9, TIM_Channel_2, TIM_CCx_Disable);
}

/*******************************************************************************
* Function Name   : init_dribbler
* Description	  : 初始化吸球功能
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_dribbler(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u16 prescale = 0;
	u32 pwm_period;

	/* set dir */
	MOTOR_BALL_BRK_HI();
	
#if (NEW_DRIBBLE_MOTOR_DIR)
	MOTOR_BALL_DIR_LOW();
#else
	MOTOR_BALL_DIR_HI();
#endif

	
	/* Compute the prescaler value */
	prescale = 0;	/* no prescale, TIM11 clock is 168MHz */
	
	pwm_period = MOTOR_PWM_PERIOD;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = pwm_period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescale;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Disable);

	TIM_Cmd(TIM11, ENABLE);
}

/*******************************************************************************
* Function Name   : set_dribbler
* Description	  : 设置吸球电机的方向和力度
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void set_dribbler(u16 value, u8 dir)
{
#if (NEW_DRIBBLE_MOTOR_DIR)
	if(dir) 
	{
		MOTOR_BALL_DIR_HI();
	}
	else
	{
		MOTOR_BALL_DIR_LOW();
	}
#else
	if(dir) 
	{
		MOTOR_BALL_DIR_LOW();
	}
	else
	{
		MOTOR_BALL_DIR_HI();
	}
#endif
	
	TIM_SetCompare1(TIM11, value);
}

/*******************************************************************************
* Function Name   : is_ball_detected
* Description	  : 判断是否吸球
*					
* Input		      : None
* Output		  : None
* Return		  : 返回1检测到球  返回0没有检测到球
*******************************************************************************/
int is_ball_detected(void)
{
	if(GET_BALL_DECET_IO()) return 1;
	else return 0;
}

/*******************************************************************************
* Function Name   : start_ball_dect_int
* Description	  : 开启吸球检测
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void start_ball_dect_int(void)
{
	is_action_shoot = 1;
	NVIC_EnableIRQ(EXTI0_IRQn);		//开启红外中断。
}

/*******************************************************************************
* Function Name   : stop_ball_dect_int
* Description	  : 停止吸球检测
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void stop_ball_dect_int(void)
{
	NVIC_DisableIRQ(EXTI0_IRQn);
	is_action_shoot = 0;
}

/*******************************************************************************
* Function Name   : init_ball_dect_irq
* Description	  : 初始化吸球检测中断
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_ball_dect_irq(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Connect EXTI Line0 to PC0 pin, IR Ball detect */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);

	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearFlag(EXTI_Line0);

	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

/*******************************************************************************
* Function Name   : init_ir_pwm
* Description	  : 
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_ir_pwm(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u16 prescale = 0;
	u32 pwm_period;

	/* Compute the prescaler value */
	prescale = 0;	/* no prescale, TIM10 clock is 168MHz */
	
	pwm_period = SystemCoreClock / IR_BALL_PWM_FREQ - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = pwm_period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescale;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = SystemCoreClock / IR_BALL_PWM_FREQ * IR_BALL_PWM_DUTY / 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Disable);

	TIM_Cmd(TIM10, DISABLE);
}


/*******************************************************************************
* Function Name   : start_ir_pwm
* Description	  : 
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void start_ir_pwm(void)
{
	TIM_Cmd(TIM10, ENABLE);
}


/*******************************************************************************
* Function Name   : stop_ir_pwm
* Description	  : 
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void stop_ir_pwm(void)
{
	TIM_Cmd(TIM10, DISABLE);
}


/*******************************************************************************
* Function Name   : N2RPM
* Description	  : motor encoder speed to RPM 速度单位转换将输入的码盘速度[count/s]
*					转换为国际速度单位[rpm]
* Input		      : long speed 格/s  count/s
* Output		  : None 编码器接口TF1 TF2双边沿采样 采样计数值为实际编码器的4倍
* Return		  : rpm 转/分钟 电机最高3000rpm
*******************************************************************************/
long N2RPM(long speed )
{
	return (speed * 60) / ( 4 * ENCODER_COUNTS_PER_TURN_SET );
}


/*******************************************************************************
* Function Name   : RPM2N
* Description	  : RPM speed to motor encoder speed
*					
* Input		      : rpm 转/分钟 电机最高3000rpm
* Output		  : None
* Return		  :  格/s  count/s 
*******************************************************************************/
long RPM2N(long rpm)
{
	return (rpm * 4 * ENCODER_COUNTS_PER_TURN_SET ) / 60;
}


/*******************************************************************************
* Function Name   : V2N
* Description	  : 将轮子的速度单位从(m/s)转换到码盘的(count/s)
*					
* Input		      : float vel 速度值 单位m/s
* Output		  : None
* Return		  : 编码器速度 格/s  count/s 
*******************************************************************************/
long V2N(float vel)
{
	return vel * g_robot.kv2n;
}


/*******************************************************************************
* Function Name   : N2V
* Description	  : 将轮子的速度单位从码盘的(count/s)转换到轮子的(m/s)
*					
* Input		      :  编码器速度 格/s  count/s 4倍频后的速度
* Output		  : None
* Return		  :  速度值 单位m/s g_robot.kv2n=74037 电机最大转速3000rpm=50rps=50*1024*4=204800/74037=2.76m/s
*******************************************************************************/
float N2V(long n)
{
	return (float)n / g_robot.kv2n;
}


/*******************************************************************************
* Function Name   : init_adc
* Description	  : 初始化adc
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_adc(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	/* ---------------------- Initial ADC -------------------------*/
	/* Disable ADC 1 */
	ADC_Cmd(ADC1, DISABLE);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式 扫描模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_6Cycles;//两个采样阶段之间的延迟 6 个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频 4 分频  84/4=21MHz,	ADC 时钟最好不要超过 36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);  

	/* ADC1 configuration */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;         //扫描模式取消
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   //连续转换取消
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发  
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles); 
	ADC_Cmd(ADC1, ENABLE);
}



/*******************************************************************************
* Function Name   : get_cap_v
* Description	  : 获取电容adc值
*					
* Input		      : None
* Output		  : None
* Return		  : 16bit adc/16
*******************************************************************************/
u8 get_cap_v(void)
{
	u16 val;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles);//PB0 通道8 电容电压检测
	
	/* Enable ADC1 */
	ADC_SoftwareStartConv(ADC1);
  
	/* Check the end of ADC1 conversion */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);

	val = (u16)(ADC1->DR & 0xffff);
	val = val >> 4;

	return (u8)(val & 0xff);
}

/*******************************************************************************
* Function Name   : get_bat_v
* Description	  : 获取电池电压adc值
*					
* Input		      : None
* Output		  : None
* Return		  : 16bit adc/16
*******************************************************************************/
u8 get_bat_v(void)
{
	u16 val;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_28Cycles);//PB1 通道9 电池电压检测
	
	/* Enable ADC1 */
	ADC_SoftwareStartConv(ADC1);
  
	/* Check the end of ADC1 conversion */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);

	val = (u16)(ADC1->DR & 0xffff);
	val = val >> 4;

	return (u8)(val & 0xff);
}


/*******************************************************************************
* Function Name   : init_nf24l01_spi
* Description	  : 初始化nrf24l01 spi接口
*					
* Input		      : None
* Output		  : None
* Return		  : 16bit adc/16
*******************************************************************************/
void init_nf24l01_spi(void)
{
	SPI_InitTypeDef  SPI_InitStructure;

	NRF_CE_LOW();
	NRF_NCS_HIGH();

	/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //5.25MHz SPI CLK

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(NRF24L01_SPI, &SPI_InitStructure);

	/* Enable the sFLASH_SPI  */
	SPI_Cmd(NRF24L01_SPI, ENABLE);
}


/*******************************************************************************
* Function Name   : nrf_spi_wr_rd
* Description	  : nrf24l01 spi read and write one byte.
*					
* Input		      : None
* Output		  : None
* Return		  : 16bit adc/16
*******************************************************************************/

u8 nrf_spi_wr_rd(u8 input)
{
	while (RESET == SPI_GetFlagStatus(NRF24L01_SPI, SPI_FLAG_TXE));   // 等待数据传输完成	
	SPI_SendData(NRF24L01_SPI, input);
	while (RESET == SPI_GetFlagStatus(NRF24L01_SPI, SPI_FLAG_RXNE));  // 等待数据接收完成
	return (SPI_ReceiveData(NRF24L01_SPI) & 0xff);
}


/*******************************************************************************
* Function Name   : read_dip_sw
* Description	  : get robot dip switcher
*					
* Input		      : None
* Output		  : None
* Return		  : 16bit adc/16
*******************************************************************************/
void read_dip_sw(u8 *freq, u8 *num, u8 *mode)
{
	u16 dat = 0;
	u8 i;
	
	DIS_INT();

	/* load dip sw bit */
	DIP_SW_LD_LOW();
	wait_us(5);
	DIP_SW_CLK_LOW();
	DIP_SW_LD_HI();

	/* load data */
	for(i = 0; i < 16; i++)
	{
		dat = dat >> 1;
		if(GET_DIP_SW_D_IO()) dat = dat | 0x8000;
		
		DIP_SW_CLK_HI();
		wait_us(1);
		DIP_SW_CLK_LOW();		
		wait_us(1);
	}

	EN_INT();
	
	/* calualte the num and freq from dip set */
	/* BECARE: the dip sw bit 4 is num and freq point LSB, and bit 1 is MSB */
	*mode = (u8)((dat >> 13) & 0x7);
	*freq = (u8)((dat >> 4) & 0xf);
	*num = (u8)((dat >> 0) & 0xf) + 1; //机器号为拨码盘加1

	/* change rf frq channel to 24l01 freq */
	switch(*freq)
	{
		case 0:
			*freq = 95;
			break;
		case 1:
			*freq = 73;
			break;
		case 2:
			*freq = 75;
			break;
		case 3:
			*freq = 77;
			break;
		case 4:
			*freq = 79;
			break;
		case 5:
			*freq = 81;
			break;
		case 6:
			*freq = 85;
			break;
		case 7:
			*freq = 89;
			break;
		case 8:
			*freq = 101;
			break;
		case 9:
			*freq = 103;
			break;
		case 10:
			*freq = 93;
			break;
		case 11:
			*freq = 105;
			break;
		case 12:
			*freq = 109;
			break;
		case 13:
			*freq = 113;
			break;
		case 14:
			*freq = 117;
			break;
		case 15:
			*freq = 121;
			break;
		default:
			*freq = 93;
			break;
	}
}


/*******************************************************************************
* Function Name   : is_infra_broken
* Description	  : 
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
int is_infra_broken(void)
{
	return 0;
}

/*******************************************************************************
* Function Name   : is_power_low
* Description	  : 检测电池电压是否过低，若低于报警电压，返回1，若低于强制停止的电压，返回2，
*					电压正常返回0
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
int is_power_low(void)
{
  	/* compare battery power AD value with low power threashold */
	static int powerarr[5] = {0};
 	static int num = 0;
	static int avepower;
	static int limpower = 5;
	static u8 is_fifo_full = 0;
	int i,u,d;
	u8 bat_v;
	int ret = 0;
  	
   	bat_v = get_bat_v(); 
   	
   	powerarr[num++] = bat_v;
	if(num == 5) 
	{
		num = 0;
		is_fifo_full = 1;
	}

	u = 0;
	d = 0;
	
	if(is_fifo_full)
	{
		for(i = 1; i < 5; i++)
		{
			if(powerarr[i] > powerarr[u]) u = i;
			if(powerarr[i] < powerarr[d]) d = i;			
		}
        
		if(powerarr[u] > (avepower + limpower)) powerarr[u] = avepower;
		if(powerarr[d] < (avepower - limpower)) powerarr[d] = avepower;
        
        avepower = powerarr[0];
		
        for(i = 1; i < 5; i++)
		{
			avepower = avepower + powerarr[i];
		}

		avepower = avepower / 5;
		bat_v = (u8)avepower;

		if(avepower < (int)WARNING_POWER_D)
		{
  			if(avepower <= (int)FORCESTOP_POWER_D)
  			{
				ret = 2;
  			}
			ret = 1;
    	}
	}
	else 
	{
		avepower = (powerarr[0] + powerarr[1] + powerarr[2] + powerarr[3]) / 4;
	}

	DIS_INT();
	g_robot.bat_v = bat_v;
	g_robot.bat_v_f = bat_v  * BAT_V_ADC_GAIN;
	EN_INT();
	
    return ret;
}

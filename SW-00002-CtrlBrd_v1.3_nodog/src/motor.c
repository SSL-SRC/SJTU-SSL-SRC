 /********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    motor.c          
* 功能描述:    
           MPU6050陀螺仪驱动程序
* 版本      作者             时间          状态
* V1.0      chenshouxian     2015.08.08    创建文件
* V1.1      sunlifeng        2016.03.2     修改文件
*****************************************************************************
*****************************************************************************/ 
#include "motor.h"
#include "pid.h"
#include "robot.h"
#include "misc.h"

/* encoder tim tab, 0 for wheel, 1 for wheel 2 ... */
TIM_TypeDef * encoder_tab[4] = {TIM5, TIM3, TIM4, TIM2};  //encoder1 encoder2 encoder3 encoder4 

u8 is_motor_run = 0;



/*******************************************************************************
* Function Name   : init_one_encoder
* Description	  : 定时器编码器初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/

void init_one_encoder(TIM_TypeDef* TIMx, u8 dir)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* Timer configuration in Encoder mode */
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // 预分频 不分频 TIM2~5 APB1 42M 42*2=84Mhz 计数器CNT频率=84M/(0+1)=84
	TIM_TimeBaseStructure.TIM_Period = 65535;   //自动装载值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //定时器时钟频率与数字滤波器采用频率(ETR,TIx)相等
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIMx, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising,
		TIM_ICPolarity_Rising);//编码器模式3计数器在TI1FP1和TI2FP2边沿计数 上升沿触发

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  //上升沿
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//通道配置为输入 IC1映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	if(dir == 0)
	{
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	}
	else
	{
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;  //通道1 encoder_A 根据电机方向上升沿捕获或者下降沿捕获
	}
	TIM_ICInit(TIMx, &TIM_ICInitStructure);  //假设电机向前转动为encoder_A领先则设置为上升沿捕获，如果encoder_B领先则设置通道1位下降沿捕获

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;    //通道2 encoder_B上升沿捕获
	TIM_ICInit(TIMx, &TIM_ICInitStructure);

	TIMx->CNT = 0;

	/* TIM enable counter */
	TIM_Cmd(TIMx, DISABLE);
}


/*******************************************************************************
* Function Name   : init_test_speed_timer
* Description	  : tim7定时器初始化用于计算速度时间
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void init_test_speed_timer(void)
{
	/* TIM7_ARR is not buffered, one pulse mode */
	TIM7->CR1 = 0x0008;                 //  TIM7APB1 42M 42*2=84Mhz 
	TIM7->CR2 = 0x0000;

	/* disable INT */
	TIM7->DIER = 0x0;

	/* prescaler X42, 1MHz clock  2Mclock */
	TIM7->PSC = (FREQ_PCLK_1 / ENCODER_TIM_CLK_FREQ) - 1;  //CNT频率 = 84M/(1+psc)=84/(168/4/1M)=2M

	TIM7->ARR = 0xffff;

	/* reload ARR and PSC */
	TIM7->EGR = 0x1;

	TIM7->CNT = 0x0;

	/* clear UIF bit */
	TIM7->SR = 0x0;
}


/*******************************************************************************
* Function Name   : init_encoder
* Description	  : 编码器捕获的定时器初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void init_encoder(void)
{
	/* initial encoder TIM */
	init_one_encoder(encoder_tab[0], 1); //TIM5
	init_one_encoder(encoder_tab[1], 1); //TIM3
	init_one_encoder(encoder_tab[2], 1); //TIM4
	init_one_encoder(encoder_tab[3], 1); //TIM2

	/* initial TIM7 as the timer for test speed */
	init_test_speed_timer();
}
 
/*******************************************************************************
* Function Name   : start_encoder
* Description	  : 开始计数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void start_encoder(void)
{
	/* Disable the TIM Counter */
	TIM2->CR1 &= (u16)~TIM_CR1_CEN;
	TIM3->CR1 &= (u16)~TIM_CR1_CEN;
	TIM4->CR1 &= (u16)~TIM_CR1_CEN;
	TIM5->CR1 &= (u16)~TIM_CR1_CEN;

	/* clear cnt */
	TIM2->CNT = 0;
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	TIM5->CNT = 0;

	/* start measurement timer, TIM 7 */
	TIM7->CNT = 0x0;
	TIM7->SR = 0x0;
	TIM7->CR1 |= 0x1;   //tim7启动计数

	/* start encoder */
	TIM2->CR1 |= TIM_CR1_CEN;  //开始计数
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM5->CR1 |= TIM_CR1_CEN;
}
 

/*******************************************************************************
* Function Name   : update_encoder
* Description	  : 编码器计算 速度计算
*					2ms调用过一次
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void update_encoder(int *speed)
{
	u8 i;
	u16 time;
	s16 encoder_cnt;
	TIM_TypeDef* TIMx;
	float tmp_f;
	
	/* Disable the TIM Counter */
	TIM7->CR1 &= (u16)~TIM_CR1_CEN;
	TIM2->CR1 &= (u16)~TIM_CR1_CEN;
	TIM3->CR1 &= (u16)~TIM_CR1_CEN;
	TIM4->CR1 &= (u16)~TIM_CR1_CEN;
	TIM5->CR1 &= (u16)~TIM_CR1_CEN;

	time = TIM7->CNT;
	if(time == 0) 
	{
		/* timer overflow */
		time = 0xffff;
	}

	for(i = 0; i < 4; i++)  //电机正向计数器增加 反向计数器从0xffff开始递减
	{	
		TIMx = encoder_tab[i];
		encoder_cnt = (s16)(TIMx->CNT & 0xffff);  //TF1 TF2双向双边沿计数 计数值为编码器脉冲的4倍
		tmp_f = (float)encoder_cnt * ((float)ENCODER_TIM_CLK_FREQ *2 / (float)time);  //count/s tim7 计数时钟2M 需要乘以2
		
		*(speed + i) = (int)tmp_f;
	}
	
	start_encoder();//计数器清0 重新计数 
}

/******************************************************************************
 * stop_encoder: 
 *
 * Parameter:
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *    2015/09/10, Shouxian Chen create this function
 * 
 ******************************************************************************/
void stop_encoder()
{
	/* Disable the TIM Counter */
	TIM7->CR1 &= (u16)~TIM_CR1_CEN;
	TIM2->CR1 &= (u16)~TIM_CR1_CEN;
	TIM3->CR1 &= (u16)~TIM_CR1_CEN;
	TIM4->CR1 &= (u16)~TIM_CR1_CEN;
	TIM5->CR1 &= (u16)~TIM_CR1_CEN;
}

/******************************************************************************
 * set_motor_dir: set motor direction
 *
 * Parameter:
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *    2015/09/09, Shouxian Chen create this function
 * 
 ******************************************************************************/
void set_motor_dir(u8 m1, u8 m2, u8 m3, u8 m4)
{
	u16 reset = 0;
	u16 set = 0;
	
	if(m1) set |= GPIO_Pin_0;
	else reset |= GPIO_Pin_0;

	if(m2) set |= GPIO_Pin_1;
	else reset |= GPIO_Pin_1;

	if(m3) set |= GPIO_Pin_2;
	else reset |= GPIO_Pin_2;

	if(m4) set |= GPIO_Pin_3;
	else reset |= GPIO_Pin_3;

	GPIOE->BSRRL = set;  //1 对相应位置1
	GPIOE->BSRRH = reset; //1 对相应位清0
}


/*******************************************************************************
* Function Name   : set_motor_break
* Description	  : 设置电机刹车
*					
* Input		      : u8 m1, u8 m2, u8 m3, u8 m4 4个电机刹车值 brake active low 低电平刹车
* Output		  : None
* Return		  : None.
*******************************************************************************/
void set_motor_break(u8 m1, u8 m2, u8 m3, u8 m4)
{
	u16 reset = 0;
	u16 set = 0;
	
	if(m1) set |= GPIO_Pin_4;
	else reset |= (GPIO_Pin_4 << 16);

	if(m2) set |= GPIO_Pin_5;
	else reset |= (GPIO_Pin_5 << 16);

	if(m3) set |= GPIO_Pin_6;
	else reset |= (GPIO_Pin_6 << 16);

	if(m4) set |= GPIO_Pin_7;
	else reset |= (GPIO_Pin_7 << 16);

	GPIOE->BSRRL = set;   //对应位置位
	GPIOE->BSRRH = reset;  //对应位清0
}

/*******************************************************************************
* Function Name   : set_motor_pwm
* Description	  : 设置电机pwm 比较寄存器值
*					
* Input		      : u16 m1, u16 m2, u16 m3, u16 m4 4个电机pwm比较寄存器值
* Output		  : None
* Return		  : None.
*******************************************************************************/
void set_motor_pwm(u16 m1, u16 m2, u16 m3, u16 m4)
{
	TIM1->CCR1 = m1;
	TIM1->CCR2 = m2;
	TIM1->CCR3 = m3;
	TIM1->CCR4 = m4;
}

 /*******************************************************************************
* Function Name   : init_motor_pwm_tim
* Description	  : 电机pwm初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void init_motor_pwm_tim(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u16 prescale = 0;
	u32 pwm_period;

	/* Compute the prescaler value */
	prescale = 0;	/* no prescale, TIM1 clock is 168MHz  APB2 84Mhz *2=168*/
	
	pwm_period = MOTOR_PWM_PERIOD;  //周期25Khz

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = pwm_period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescale;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	/* TIM1 DISABLE counter */
	TIM_Cmd(TIM1, DISABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/*******************************************************************************
* Function Name   : init_motor
* Description	  : 电机相关的初始化
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void init_motor(void)
{
	/* initial TIM1 for motor PWM */
	init_motor_pwm_tim();

	set_motor_break(1, 1, 1, 1);
	set_motor_dir(0, 0, 0, 0);
	set_motor_pwm(0, 0, 0, 0);
	
	/* initial encoder */
	init_encoder();

	is_motor_run = 0;
}

/*******************************************************************************
* Function Name   : update_motor
* Description	  : 中断周期调用,pid计算,速度计算等
*					2ms调用过一次
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void update_motor(void)
{
	u8 i;
	pid_t *pid_h;
	int pwm_val;
	u8 motor_dir[CHANNEL_NUM];
	u16	motor_pwm[CHANNEL_NUM];
	int cur_speed[CHANNEL_NUM];   //count/s

	if(is_motor_run == 0) return;
	
	/* update the speed */
	update_encoder(cur_speed);
	
	/* calcualte the pid output */
	for(i = 0; i < CHANNEL_NUM; i++)
	{
		g_robot.wheels[i].cur_speed = cur_speed[i];
		
		pid_h = &(g_robot.wheels[i].pid);
		//pwm_val = pid_step(pid_h, cur_speed[i], g_robot.bat_v);
		pwm_val = pid_step(pid_h, cur_speed[i], 16.0f);
		
		if(pwm_val < 0) 
		{
			motor_dir[i] = 1;
			motor_pwm[i] = -pwm_val;
		}
		else
		{
			motor_dir[i] = 0;
			motor_pwm[i] = pwm_val;
		}
	}

	/* update the motor ctrl */
	set_motor_dir(motor_dir[0], motor_dir[1], motor_dir[2], motor_dir[3]);
	set_motor_pwm(motor_pwm[0], motor_pwm[1], motor_pwm[2], motor_pwm[3]);
}


/*******************************************************************************
* Function Name   : start_motor
* Description	  : 启动电机开始编码器计数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/ 
void start_motor(void)
{
	u8 i;
	
	for(i = 0; i < CHANNEL_NUM; i++)
	{
		pid_reinit(&g_robot.wheels[i].pid);
	}

	start_encoder();

	DIS_INT();
	is_motor_run = 1;
	EN_INT();

	TIM_Cmd(TIM1, ENABLE);
}


/*******************************************************************************
* Function Name   : stop_motor
* Description	  : 停止电机
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/ 
void stop_motor(void)
{
	TIM_Cmd(TIM1, DISABLE);//停止PWM输出

	DIS_INT();
	is_motor_run = 0;
	EN_INT();

	set_motor_pwm(0, 0, 0, 0);
	set_motor_break(1, 1, 1, 1);
	set_motor_dir(0, 0, 0, 0);
	stop_encoder();
}

/*******************************************************************************
* Function Name   : get_motor_speed
* Description	  : 获取电机速度	轮子的速度单位rpm			
* Input		      : int *speed
* Output		  : None
* Return		  : None.
*******************************************************************************/ 

void get_motor_speed(int *speed)
{
	u8 i;
	int cur_speed[CHANNEL_NUM];

	DIS_INT();
	for(i = 0; i < CHANNEL_NUM; i++)
	{
		cur_speed[i] = g_robot.wheels[i].cur_speed;
	}
	EN_INT();

	for(i = 0; i < CHANNEL_NUM; i++)
	{
		*(speed + i) = N2RPM(cur_speed[i]);
	}
}


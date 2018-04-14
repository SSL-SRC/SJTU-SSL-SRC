 /********************************************************************************
 ********************************************************************************
 * 权限: 杭州南江机器人股份有限公司
 * 文件名:	   action.c		 
 * 功能描述: 
 *		   机器人动作执行函数
 * 版本   作者		     时间		   状态
 * V1.0   chenshouxian	 2015.09.10  创建文件
 * V1.1   sunlifeng	     2016.04.8   修改文件
 *****************************************************************************
 *****************************************************************************/
#include <math.h>
#include "arm_math.h"
#include "action.h"
#include "timer.h"
#include "cfg.h"
#include "gpio.h"
#include "robot.h"
#include "misc.h"
#include "motor.h"

char shooter;
char g_is_shoot_set = 0;
shooter_t g_shooter[ SHOOTER_NUM ];

vu8 is_action_shoot = 0;
extern unsigned char identify_success;



/*******************************************************************************
* Function Name   : update_shooter
* Description	  : 更新计数 1ms减1
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int update_shooter(void)
{
	int i; 
	
	for(i = 0; i < SHOOTER_NUM; i++)
	{
		if(g_shooter[i].count_down > 0)
			g_shooter[i].count_down--;
		else
			g_shooter[i].count_down = 0;
	}
	return 0;
}

/*******************************************************************************
* Function Name   : set_test_shooter
* Description	  : 自测模式下调用
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int set_test_shooter(void)
{
	int i; 
	
	for(i = 0; i < SHOOTER_NUM; i++)
	{
		g_shooter[i].count_down = 0;
	}
	return 0;
}

/*******************************************************************************
* Function Name   : shooter_off
* Description	  : 平射挑射功能关闭
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int shooter_off(void)
{
	u8 i;
	
	// shoot & chip disable 
	shoot_off();
	chip_off();
	for(i = 0; i < SHOOTER_NUM; i++)
	{
		g_shooter[i].count_down = 0;
	}

	return 0;
}


/*******************************************************************************
* Function Name   : set_shooter
* Description	  : 执行平射挑射动作力度
*	执行射门指令,形参channel为1时平射,为0时挑射;形参value为射门力度范围,执行射门前检查电容电压和
*    力度大小,只有电容电压足够且射门力度不为0时才能射门.
* Input		      : int channel, int value
* Output		  : None
* Return		  : None.
*******************************************************************************/
int set_shooter( int channel, int value )
{
	if(value <= 0) return 0;
	
	if(g_shooter[ channel ].count_down ) 
	{
		return -1;
	}

	if(GET_CAP_LOW_IO())
	{
		g_robot.is_cap_low = 0;
	}
	else
	{
		g_robot.is_cap_low = 1;
	}

	if((!g_robot.is_cap_low) && value )
	{
		g_shooter[ channel ].strength = value;
		g_shooter[ channel ].count_down = SHOOTER_INTERVAL;

		switch ( channel )
		{
			// channel 0 is shoot , high is active , 
			case SHOOT : 
			{
				shoot_on(value);
				shooter = 0x02;
				break;
			}
			
			// channel 1 is chip ,  high is active , 
			case CHIP :
			{
				chip_on(value);
				shooter = 0x01;
				break;
			}

			default:
			{
				chip_off();
				shoot_off();
				return -1;
			}
		}
	}

 	return 0;
}

/*******************************************************************************
* Function Name   : init_shooter
* Description	  : 初始化平射挑射
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int init_shooter(void)
{
	shooter_off();
	return 0;
}


/*******************************************************************************
* Function Name   : infra_handle
* Description	  : 红外中断函数,用于射门信号的执行.收到射门信号时,如果红外没有检测到球在嘴巴里,
*                   则开启红外中断.球进入车前嘴巴内,触发中断,如果设置了射门力度则执行射门指令			
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void infra_handle (void)
{
	u8 has_do_shoot = 0;

	if(is_action_shoot == 0) goto END;

	if(g_robot.shoot > 0)                   //判定射门力度是否是否大于0
	{
		set_shooter(SHOOT, g_robot.shoot);	//执行平射指令
		g_robot.shoot = 0;                  //执行完后将平射指令清零
		has_do_shoot = 1;
	}
	else
	{
		g_robot.shoot = 0;
	}

	if(g_robot.chip > 0)
	{
		if(has_do_shoot == 0)
		{
			set_shooter(CHIP, g_robot.chip);  	//执行挑射指令
		}
		g_robot.chip=0;                 	//执行完后将挑射指令清零
	}
	else
	{
		g_robot.chip=0;
	}

END:
	is_action_shoot = 0;
	NVIC_DisableIRQ(EXTI0_IRQn);
}

/*******************************************************************************
* Function Name   : do_dribbler
* Description	  : 设置吸球档位力度
*		给带球电机力度参数（g_robot.dribbler.set）赋值，输入为带球力度档数，档数分为0-1-2-3			
* Input		      : int dribbler
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_dribbler( int dribbler )
{
  int dribbler_temp;
	u8 dir = 0;

	if(dribbler < 0) 
	{
		dribbler_temp = -dribbler;
		dir = 1;
	}
	else 
	{
		dribbler_temp = dribbler;
		dir = 0;
	}		

    if(dribbler_temp == 0)
    {
        g_robot.dribbler = 0;
    }
    else if(dribbler_temp == 1)
    {
        g_robot.dribbler = MOTOR_PWM_PERIOD * 0.3;
    }
    else if(dribbler_temp == 2)
    {
        g_robot.dribbler = MOTOR_PWM_PERIOD * 0.4;
    }
    else if(dribbler_temp == 3)
    {
        g_robot.dribbler = MOTOR_PWM_PERIOD * 0.5;
    }
	else
	{
		g_robot.dribbler = 0;
	}

	set_dribbler(g_robot.dribbler, dir);
  
	return 0;
}

/*******************************************************************************
* Function Name   : do_move
* Description	  : 执行运动函数
*					设定小车的x,y,z的速度，形参为x,y,z速度，速度单位为国际单位
* Input		      : int speed_x, int speed_y, int speed_rot x,y单位[cm/s] rot 单位0.025rad/s
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_move( int speed_x, int speed_y, int speed_rot )
{
	int i = 0;
 
	/* 线速度 vx, vy, vz are all measured in m/s */
	float vx = (float)(speed_x) / 100;
	float vy = (float)(speed_y)  / 100;    //单位[m/s]
	float vz = (float)(speed_rot) * 0.025f * WHEEL_CENTER_OFFSET; //V=2*pi*r/t = w*r 单位[m/s]

	/* 各轮子线速度(m/s)设定值 */
	for( i = 0; i < CHANNEL_NUM; i++ )
	{
		/* trasnform wheel angle */
		g_robot.wheels[i].speed = ( g_robot.sin_angle[ i ] * vx + g_robot.cos_angle[ i ] * vy + vz );
		g_robot.wheels[i].set = V2N(g_robot.wheels[i].speed);//线速度转换为编码器速度
		
	}
  
	/* change wheels' speed set point, with dis_int() */
	DIS_INT();
	for( i = 0; i < CHANNEL_NUM; i++ )
	{
		
		g_robot.wheels[i].pid.set = g_robot.wheels[i].set;
	}
	EN_INT();
	
	return 0;
}

/*******************************************************************************
* Function Name   : do_acc_handle_move
* Description	  : 执行运动
*	限定小车xy方向上合加速度值小于MAX_ACC,并且设定小车x,y,z方向上移动速度分别为	
*   speed_x, speed_y, speed_rot，单位为国际单位		max_rot=256/40=6.4rad/s	
* Input		      : int speed_x,int speed_y,int speed_rot
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_acc_handle_move(int speed_x,int speed_y,int speed_rot)
{
	static float last_speed_x = 0;
	static float last_speed_y = 0;

	float acc_x = 0;
	float acc_y = 0;
	float acc_whole = 0;
	float sin_x = 0;
	float sin_y = 0;
	float tmp_float;
	
	acc_x = speed_x - last_speed_x;   //x方向加速度计算 时间单位为通讯包周期
	acc_y = speed_y - last_speed_y;
	acc_whole = acc_x * acc_x + acc_y * acc_y ;

	arm_sqrt_f32(acc_whole, &tmp_float);
	acc_whole = tmp_float + 0.001f;
	
	sin_x = acc_x / acc_whole;
	sin_y = acc_y / acc_whole;

   	if(acc_whole > MAX_ACC)
  	{
		acc_whole = MAX_ACC;
		acc_x = acc_whole * sin_x;
		acc_y = acc_whole * sin_y;
		speed_x = ceil(last_speed_x + acc_x);
		speed_y = ceil(last_speed_y + acc_y); 
  	}

	do_move(speed_x,speed_y,speed_rot);
	last_speed_x = speed_x;
	last_speed_y = speed_y;
  
	return 0;
}

/*******************************************************************************
* Function Name   : do_shoot
* Description	  : 执行平射命令
*	形参shoot为1时,若嘴里有球才能平射,若嘴里没球,则不射门重新开启红
*  外中断.若形参shoot和chip都为零,则关闭红外中断
* Input		      : int shoot, int chip
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_shoot( int shoot, int chip )
{
	/* 先保存平射指令 */
	g_robot.shoot = shoot;
  
	if( shoot )
	{
		/* 先清红外检测中断，再判断球状态，如果没有球，再启动中断检测，顺序不能
			变，否则可能漏检测 */
		EXTI_ClearFlag(EXTI_Line0);
		NVIC_ClearPendingIRQ(EXTI0_IRQn);

		if ( is_ball_detected() == 1 )
		{
		   set_shooter( SHOOT, shoot );
		}
		else
		{
			start_ball_dect_int();
		}
	}
	else if((chip == 0) && (shoot == 0))      //没有射门直也没有挑射值得时候关闭中断 
	{   	
		stop_ball_dect_int();
	}
  
	return 0;
}

/*******************************************************************************
* Function Name   : do_chip
* Description	  : 执行挑射命令
*		形参chip为1时,若嘴里有球才能平射,若嘴里没球,则不射门重新开启红
*       外中断.若形参shoot和chip都为零,则关闭红外中断
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_chip(int shoot, int chip )
{
  /* 先保存挑射指令 */
	g_robot.chip = chip;
    
  	if( chip )
	{	
		/* 先清红外检测中断，再判断球状态，如果没有球，再启动中断检测，顺序不能
			变，否则可能漏检测 */
		EXTI_ClearFlag(EXTI_Line0);
		NVIC_ClearPendingIRQ(EXTI0_IRQn);

		if( is_ball_detected() == 1 )
		{
			set_shooter( CHIP, chip );
		}
		else
		{
			start_ball_dect_int();
		}
	}
  	else if((chip == 0) && (shoot == 0))
	{
		stop_ball_dect_int();
	}

	return 0;
}

/*******************************************************************************
* Function Name   : on_robot_command
* Description	  : 执行控制器命令运动机器人
*					
* Input		      :  packet_robot_t *packet
* Output		  : None
* Return		  : None.
*******************************************************************************/
int on_robot_command( packet_robot_t *packet )
{   
    static int comm_count;

	/*count the right packet */
	comm_count++;
	if(comm_count < 15)
	{
		COMM_LED_ON();
	}
	else 
	{
		COMM_LED_OFF();
	}
	if(comm_count > 30)	comm_count=0;

    if(( g_robot.mode == NORMAL_MODE ) && (identify_success == 1)) //认证成功才能运行
    {
		do_dribbler( packet->dribbler );//设置控制档位
		 
#ifdef ENABLE_SHOOTER
		do_shoot(packet->shoot, packet->chip);//平射
		do_chip(packet->shoot, packet->chip);//挑射
 #endif
		do_acc_handle_move(packet->speed_x, packet->speed_y, packet->speed_rot);
 
    }
   	
	return 0;
}

/*******************************************************************************
* Function Name   : do_robot_command
* Description	  : 执行控制器命令运动机器人 手柄模式下调用
*					
* Input		      :  packet_robot_t *packet
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_robot_command( packet_robot_t *packet )
{   
    static int count;

	/*count the right packet */
	count++;
	if(count < 15)
	{
		COMM_LED_ON();
	}
	else 
	{
		COMM_LED_OFF();
	}
	if(count > 30)	count=0;

    if( g_robot.mode == JOYSTICK_MODE ) //手柄模式不需要CPUID认证
    {
		do_dribbler( packet->dribbler );//设置控制档位
		 
#ifdef ENABLE_SHOOTER
		do_shoot(packet->shoot, packet->chip);//平射
		do_chip(packet->shoot, packet->chip); //挑射
 #endif
		do_acc_handle_move(packet->speed_x, packet->speed_y, packet->speed_rot);
 
    }
   	
	return 0;
}

/*******************************************************************************
* Function Name   : EXTI0_IRQHandler
* Description	  : 中断函数
*					
* Input		      :  packet_robot_t *packet
* Output		  : None
* Return		  : None.
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
	infra_handle();
}



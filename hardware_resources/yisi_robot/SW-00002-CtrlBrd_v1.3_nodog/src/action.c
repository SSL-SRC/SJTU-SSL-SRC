 /********************************************************************************
 ********************************************************************************
 * Ȩ��: �����Ͻ������˹ɷ����޹�˾
 * �ļ���:	   action.c		 
 * ��������: 
 *		   �����˶���ִ�к���
 * �汾   ����		     ʱ��		   ״̬
 * V1.0   chenshouxian	 2015.09.10  �����ļ�
 * V1.1   sunlifeng	     2016.04.8   �޸��ļ�
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
* Description	  : ���¼��� 1ms��1
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
* Description	  : �Բ�ģʽ�µ���
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
* Description	  : ƽ�����书�ܹر�
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
* Description	  : ִ��ƽ�����䶯������
*	ִ������ָ��,�β�channelΪ1ʱƽ��,Ϊ0ʱ����;�β�valueΪ�������ȷ�Χ,ִ������ǰ�����ݵ�ѹ��
*    ���ȴ�С,ֻ�е��ݵ�ѹ�㹻���������Ȳ�Ϊ0ʱ��������.
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
* Description	  : ��ʼ��ƽ������
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
* Description	  : �����жϺ���,���������źŵ�ִ��.�յ������ź�ʱ,�������û�м�⵽���������,
*                   ���������ж�.����복ǰ�����,�����ж�,�������������������ִ������ָ��			
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void infra_handle (void)
{
	u8 has_do_shoot = 0;

	if(is_action_shoot == 0) goto END;

	if(g_robot.shoot > 0)                   //�ж����������Ƿ��Ƿ����0
	{
		set_shooter(SHOOT, g_robot.shoot);	//ִ��ƽ��ָ��
		g_robot.shoot = 0;                  //ִ�����ƽ��ָ������
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
			set_shooter(CHIP, g_robot.chip);  	//ִ������ָ��
		}
		g_robot.chip=0;                 	//ִ���������ָ������
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
* Description	  : ��������λ����
*		�����������Ȳ�����g_robot.dribbler.set����ֵ������Ϊ�������ȵ�����������Ϊ0-1-2-3			
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
* Description	  : ִ���˶�����
*					�趨С����x,y,z���ٶȣ��β�Ϊx,y,z�ٶȣ��ٶȵ�λΪ���ʵ�λ
* Input		      : int speed_x, int speed_y, int speed_rot x,y��λ[cm/s] rot ��λ0.025rad/s
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_move( int speed_x, int speed_y, int speed_rot )
{
	int i = 0;
 
	/* ���ٶ� vx, vy, vz are all measured in m/s */
	float vx = (float)(speed_x) / 100;
	float vy = (float)(speed_y)  / 100;    //��λ[m/s]
	float vz = (float)(speed_rot) * 0.025f * WHEEL_CENTER_OFFSET; //V=2*pi*r/t = w*r ��λ[m/s]

	/* ���������ٶ�(m/s)�趨ֵ */
	for( i = 0; i < CHANNEL_NUM; i++ )
	{
		/* trasnform wheel angle */
		g_robot.wheels[i].speed = ( g_robot.sin_angle[ i ] * vx + g_robot.cos_angle[ i ] * vy + vz );
		g_robot.wheels[i].set = V2N(g_robot.wheels[i].speed);//���ٶ�ת��Ϊ�������ٶ�
		
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
* Description	  : ִ���˶�
*	�޶�С��xy�����Ϻϼ��ٶ�ֵС��MAX_ACC,�����趨С��x,y,z�������ƶ��ٶȷֱ�Ϊ	
*   speed_x, speed_y, speed_rot����λΪ���ʵ�λ		max_rot=256/40=6.4rad/s	
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
	
	acc_x = speed_x - last_speed_x;   //x������ٶȼ��� ʱ�䵥λΪͨѶ������
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
* Description	  : ִ��ƽ������
*	�β�shootΪ1ʱ,�������������ƽ��,������û��,���������¿�����
*  ���ж�.���β�shoot��chip��Ϊ��,��رպ����ж�
* Input		      : int shoot, int chip
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_shoot( int shoot, int chip )
{
	/* �ȱ���ƽ��ָ�� */
	g_robot.shoot = shoot;
  
	if( shoot )
	{
		/* ����������жϣ����ж���״̬�����û�����������жϼ�⣬˳����
			�䣬�������©��� */
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
	else if((chip == 0) && (shoot == 0))      //û������ֱҲû������ֵ��ʱ��ر��ж� 
	{   	
		stop_ball_dect_int();
	}
  
	return 0;
}

/*******************************************************************************
* Function Name   : do_chip
* Description	  : ִ����������
*		�β�chipΪ1ʱ,�������������ƽ��,������û��,���������¿�����
*       ���ж�.���β�shoot��chip��Ϊ��,��رպ����ж�
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_chip(int shoot, int chip )
{
  /* �ȱ�������ָ�� */
	g_robot.chip = chip;
    
  	if( chip )
	{	
		/* ����������жϣ����ж���״̬�����û�����������жϼ�⣬˳����
			�䣬�������©��� */
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
* Description	  : ִ�п����������˶�������
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

    if(( g_robot.mode == NORMAL_MODE ) && (identify_success == 1)) //��֤�ɹ���������
    {
		do_dribbler( packet->dribbler );//���ÿ��Ƶ�λ
		 
#ifdef ENABLE_SHOOTER
		do_shoot(packet->shoot, packet->chip);//ƽ��
		do_chip(packet->shoot, packet->chip);//����
 #endif
		do_acc_handle_move(packet->speed_x, packet->speed_y, packet->speed_rot);
 
    }
   	
	return 0;
}

/*******************************************************************************
* Function Name   : do_robot_command
* Description	  : ִ�п����������˶������� �ֱ�ģʽ�µ���
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

    if( g_robot.mode == JOYSTICK_MODE ) //�ֱ�ģʽ����ҪCPUID��֤
    {
		do_dribbler( packet->dribbler );//���ÿ��Ƶ�λ
		 
#ifdef ENABLE_SHOOTER
		do_shoot(packet->shoot, packet->chip);//ƽ��
		do_chip(packet->shoot, packet->chip); //����
 #endif
		do_acc_handle_move(packet->speed_x, packet->speed_y, packet->speed_rot);
 
    }
   	
	return 0;
}

/*******************************************************************************
* Function Name   : EXTI0_IRQHandler
* Description	  : �жϺ���
*					
* Input		      :  packet_robot_t *packet
* Output		  : None
* Return		  : None.
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
	infra_handle();
}



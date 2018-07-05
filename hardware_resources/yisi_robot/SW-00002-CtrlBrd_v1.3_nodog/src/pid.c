/********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    pid.c          
* 功能描述:    
           pid控制算法
* 版本      作者             时间          状态
* V1.0      chenshouxian     2015.09.09    创建文件
*****************************************************************************
*****************************************************************************/ 

#include "cfg.h"
#include "pid.h"
#include "misc.h"


GYRO_PID_STRUCT gyro_pid = {0};

/*******************************************************************************
* Function Name   : calc_max_output
* Description	  : 计算最大输出
*					
* Input 		  : float max_torque, long speed, float bat_v
* Output		  : None
* Return		  : None.
******************************************************************************/
long calc_max_output( float max_torque, long speed, float bat_v )
{
	float Vin = max_torque / MOTOR_KI * MOTOR_R + (float)speed / MOTOR_KV;

	return MOTOR_PWM_PERIOD * Vin / bat_v;
}


/*******************************************************************************
* Function Name   : pid_init
* Description	  : pid参数计算初始化 增量式pid u(k)=u(k-1)+deta_u(k)
*					deat_u(k)=(kp+ki+kd)*e(k)-(kp+2kd)*e(k-1)+kd*e(k-2)
* Input 		  : pid_t *pid, float Kp, float Ki, float Kd 
* Output		  : None
* Return		  : None.
******************************************************************************/
void pid_init(pid_t *pid, float Kp, float Ki, float Kd )
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;

	pid->A = Kp + Ki + Kd;
	pid->B = Kp + 2 * Kd;
	pid->C = Kd;
  
  	pid->set = 0;
	
	pid->limit = MOTOR_PWM_PERIOD;
    pid->torque_limit = MOTOR_TORQUE_LIMIT;

	pid_reinit(pid);
}

/******************************************************************************
 * pid_set_point: 
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
void pid_set_point(pid_t *pid, int set_value )
{
	pid->set = set_value;
}

/******************************************************************************
 * pid_set_limit: 
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
void pid_set_limit(pid_t *pid, int limit )
{
	pid->limit = limit;
}

/*******************************************************************************
* Function Name   : pid_set_torque_limit
* Description	  : pid参数计算初始化 
*		
* Input 		  : pid_t *pid, float limit 
* Output		  : None
* Return		  : None.
******************************************************************************/
void pid_set_torque_limit(pid_t *pid, float limit )
{
	pid->torque_limit = limit;
}


/*******************************************************************************
* Function Name   : pid_step
* Description	  : pid控制计算 增量式pid u(k)=u(k-1)+deta_u(k)
*					deat_u(k)=(kp+ki+kd)*e(k)-(kp+2kd)*e(k-1)+kd*e(k-2)
* Input 		  : pid_t *pid, int cur_value, float bat_v
* Output		  : None
* Return		  : pid控制器输出.
*******************************************************************************/
int pid_step(pid_t *pid, int cur_value, float bat_v )
{
	long rpm = 0; 
	long max_output_for_torque = 0;
	int d_out;
  
	/* standard digital PID algorithm */
	pid->e3 = pid->e2;
	pid->e2 = pid->e1;
	pid->e1 = pid->set - cur_value;

	d_out = pid->A * pid->e1 - pid->B * pid->e2 + pid->C * pid->e3;
  
	pid->out = pid->out + d_out;
	
	/* output limit */
	if( pid->out > pid->limit )
		pid->out = pid->limit;
	else if( pid->out < -pid->limit )
		pid->out = -pid->limit;
  
#ifdef ENABLE_TORQUE_LIMIT
	/* perform torque limit */
	rpm = N2RPM(cur_value);
	if( rpm < 0 ) rpm = -rpm;
	max_output_for_torque = calc_max_output( pid->torque_limit, rpm, bat_v);

	/* torque limit */
	if( pid->out > max_output_for_torque )
		pid->out = max_output_for_torque;
	else if( pid->out < -max_output_for_torque )
		pid->out = -max_output_for_torque;
#endif

	return pid->out;
}


/*******************************************************************************
* Function Name   : gyro_pid_init
* Description	  : 陀螺仪闭环控制pid参数计算初始化 增量式pid u(k)=u(k-1)+deta_u(k)
*					deat_u(k)=(kp+ki+kd)*e(k)-(kp+2kd)*e(k-1)+kd*e(k-2)
* Input 		  : pid_t *pid, float Kp, float Ki, float Kd 
* Output		  : None
* Return		  : None.
******************************************************************************/
void gyro_pid_init(GYRO_PID_STRUCT *pid, float Kp, float Ki, float Kd )
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;

	pid->A = Kp + Ki + Kd;
	pid->B = Kp + 2 * Kd;
	pid->C = Kd;
  	
	pid->limit = 150;     
   
	pid->e1 = 0;
	pid->e2 = 0;
	pid->e3 = 0;
	pid->set_value = 0;   //旋转值设定为0
	pid->feedback_speed = 0;
	pid->out = 0;
}

/*******************************************************************************
* Function Name   : gyro_control_calcuate
* Description	  : pid控制计算 增量式pid u(k)=u(k-1)+deta_u(k)
*					deat_u(k)=(kp+ki+kd)*e(k)-(kp+2kd)*e(k-1)+kd*e(k-2)
* Input 		  : pid_t *pid, int cur_value, float bat_v
* Output		  : None
* Return		  : pid控制器输出.
*******************************************************************************/
int gyro_control_calcuate(GYRO_PID_STRUCT *pid)
{

	float d_out;
  
	/* standard digital PID algorithm */
	pid->e3 = pid->e2;
	pid->e2 = pid->e1;
	pid->e1 = pid->set_value - pid->feedback_speed;

	d_out = pid->A * pid->e1 - pid->B * pid->e2 + pid->C * pid->e3;
  
	pid->out = pid->out + d_out;
	
	/* output limit */
	if( pid->out > pid->limit )
		pid->out = pid->limit;
	else if( pid->out < -pid->limit )
		pid->out = -pid->limit;
  

	return pid->out;
}

/*******************************************************************************
* Function Name   : pid_get_output
* Description	  : 
* Input 		  : pid_t *pid
* Output		  : None
* Return		  : None.
******************************************************************************/
int pid_get_output(pid_t *pid)
{
	return pid->out;
}

/*******************************************************************************
* Function Name   : pid_set_param
* Description	  : pid参数计算初始化 增量式pid u(k)=u(k-1)+deta_u(k)
*					deat_u(k)=(kp+ki+kd)*e(k)-(kp+2kd)*e(k-1)+kd*e(k-2)
* Input 		  : pid_t *pid, float Kp, float Ki, float Kd 
* Output		  : None
* Return		  : None.
******************************************************************************/
void pid_set_param(pid_t *pid, float Kp, float Ki, float Kd )
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;

	pid->A = Kp + Ki + Kd;
	pid->B = Kp + 2 * Kd;
	pid->C = Kd;
}

/*******************************************************************************
* Function Name   : pid_reinit
* Description	  : pid参数计算初始化 
*		
* Input 		  : pid_t *pid
* Output		  : None
* Return		  : None.
******************************************************************************/
void pid_reinit(pid_t *pid)
{
	pid->e1 = 0;
	pid->e2 = 0;
	pid->e3 = 0;
	pid->set = 0;
}


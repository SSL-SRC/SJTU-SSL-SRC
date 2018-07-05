/********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    pid.h         
* 功能描述:    
           pid结构体定义
* 版本      作者             时间          状态
* V1.0      chenshouxian     2015.09.09    创建文件
*****************************************************************************
*****************************************************************************/ 


#ifndef __PID_H__
#define __PID_H__

typedef struct _pid_
{
	/* parameters */
	float Kp;
	float Ki;
	float Kd;

	float A;
	float B;
	float C;

	/* limit */
	long limit;
	float torque_limit; /* measured in mNm */
	
	/* status */
	float e1;          //速度误差e(k) 单位count/s
	float e2;          //e(k-1)
	float e3;          //e(k-2)

	/* input */
	volatile long set;  //速度设定值 单位count/s

	/* output */
	volatile long out;
} pid_t;

typedef struct
{
	/* parameters */
    float Kp;
	float Ki;
	float Kd;
	
	float A;
	float B;
	float C;
	
	/* limit */
	long limit;     //最大256 输出作为速度旋转的输入单位[0.025rad/s]

    float e1;          //速度误差e(k) 单位 °/s
	float e2;          //e(k-1)
	float e3;

	volatile float set_value;    //水平方向选择角速度速度值 单位 °/s
	volatile float feedback_speed;    //水平方向旋转角速度反馈值 单位 °/s
	volatile float out;              //旋转速度值 单位 0.025rad/s
}GYRO_PID_STRUCT;


extern GYRO_PID_STRUCT gyro_pid;

void pid_init(pid_t *pid, float Kp, float Ki, float Kd );
void pid_set_point(pid_t *pid, int set_value );
void pid_set_limit(pid_t *pid, int limit );
void pid_set_torque_limit(pid_t *pid, float limit );
int pid_get_output(pid_t *pid);
void pid_set_param(pid_t *pid, float Kp, float Ki, float Kd );
void pid_reinit(pid_t *pid);

int pid_step(pid_t *pid, int cur_value, float bat_v );
int gyro_control_calcuate(GYRO_PID_STRUCT *pid);
void gyro_pid_init(GYRO_PID_STRUCT *pid, float Kp, float Ki, float Kd );

#endif

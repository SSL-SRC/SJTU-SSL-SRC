/********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:    pid.h         
* ��������:    
           pid�ṹ�嶨��
* �汾      ����             ʱ��          ״̬
* V1.0      chenshouxian     2015.09.09    �����ļ�
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
	float e1;          //�ٶ����e(k) ��λcount/s
	float e2;          //e(k-1)
	float e3;          //e(k-2)

	/* input */
	volatile long set;  //�ٶ��趨ֵ ��λcount/s

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
	long limit;     //���256 �����Ϊ�ٶ���ת�����뵥λ[0.025rad/s]

    float e1;          //�ٶ����e(k) ��λ ��/s
	float e2;          //e(k-1)
	float e3;

	volatile float set_value;    //ˮƽ����ѡ����ٶ��ٶ�ֵ ��λ ��/s
	volatile float feedback_speed;    //ˮƽ������ת���ٶȷ���ֵ ��λ ��/s
	volatile float out;              //��ת�ٶ�ֵ ��λ 0.025rad/s
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

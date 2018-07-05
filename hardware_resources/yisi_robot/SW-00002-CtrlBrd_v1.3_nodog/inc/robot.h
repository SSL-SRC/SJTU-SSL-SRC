 /********************************************************************************
 ********************************************************************************
 * Ȩ��:	�����Ͻ������˹ɷ����޹�˾
 * �ļ���:	  robot.h		
 * ��������:	
		    ͷ�ļ�
 * �汾 	 ����		ʱ��		  ״̬
 * V1.0 	 shouxian	2015.9.10	�����ļ�
 * V1.1      sunlifeng  2016.2.28   �޸��ļ�
 *****************************************************************************
 *****************************************************************************/
#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "cfg.h"
#include "pid.h"
#include "typedef.h"
#include "param.h"


/***���������ֹ���ģʽѡ��***/
typedef enum _mode
{
	NORMAL_MODE = 0,     //��������ģʽ ��ͨѶ��ͨѶ
    JOYSTICK_MODE = 1,   //USB��Ϸ�ֱ�ģʽ
    BlUETOOTH_MODE = 2,  //�����ֻ�APP�ٿ�ģʽ
    SELFTEST_MODE = 3,   //�Լ�ģʽ
    DEBUG_MODE = 4       //debugģʽ usb�豸ģʽ���⴮����PCͨ�� �·����ò�����������
}mode_t;

typedef enum _wireless_mode
{
	RF_24L01 = 0,  //NRF
	RF_BTE = 1   //����
}wireless_mode_t;

typedef struct _wheel_
{
	pid_t	pid;
	volatile float speed;     //С������ϳ����ٶ�ֵ ��λ[m/s]
	volatile int set;         //pid�ٶ�����ֵ ��λcount/s
	volatile int cur_speed;   //��ǰ�ٶ� ��λ[count/s]
} wheel_t;

typedef struct _error_
{
	u64 time; /* 0:6v motor; 1:12v motor */
	int name;
} err_t;

typedef struct _robot_
{
	int num; 	/* g_robot num load from circuit configuration switch ������Ϊ������+1*/
	mode_t mode;	/* g_robot mode */
	wireless_mode_t rf_mode;	/* rf mode */
	u8 frq;	/* 2.4G rf freq */
	
	wheel_t wheels[ CHANNEL_NUM ]; //�������ӵ�pid����
	u16 dribbler;
	
	float kv2n;                    // 74037 m/s��count/s ת��ϵ�� ��λcount/m  ���ڵ����ת�ٶ�ת��Ϊ���ٶ�
	int shoot;
	int chip;
	unsigned char is_ball_detected;
	int is_cap_low;
	int is_pow_low;
	int speed_x;
	int speed_y;

	vu8 cap_v;                //���ݵ�ѹ ��λ[bit]
	vu8 bat_v;

	volatile float cap_v_f;  //�����ݵ�ѹ ��λ[v]
	volatile float bat_v_f;  //��ص�ѹ

	float sin_angle[ CHANNEL_NUM]; //���������������ߵĽǶ�����
	float cos_angle[ CHANNEL_NUM];

	err_t error[ERROR_NUM];
	
	u16 firmware_version;    //����汾

} robot_t;

int init_robot(void);
void do_run(void);
void SysTick_Handler(void);



extern robot_t g_robot;

#endif


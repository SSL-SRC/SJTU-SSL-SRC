 /********************************************************************************
 ********************************************************************************
 * 权限:	杭州南江机器人股份有限公司
 * 文件名:	  robot.h		
 * 功能描述:	
		    头文件
 * 版本 	 作者		时间		  状态
 * V1.0 	 shouxian	2015.9.10	创建文件
 * V1.1      sunlifeng  2016.2.28   修改文件
 *****************************************************************************
 *****************************************************************************/
#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "cfg.h"
#include "pid.h"
#include "typedef.h"
#include "param.h"


/***控制器几种工作模式选择***/
typedef enum _mode
{
	NORMAL_MODE = 0,     //正常比赛模式 与通讯机通讯
    JOYSTICK_MODE = 1,   //USB游戏手柄模式
    BlUETOOTH_MODE = 2,  //蓝牙手机APP操控模式
    SELFTEST_MODE = 3,   //自检模式
    DEBUG_MODE = 4       //debug模式 usb设备模式虚拟串口与PC通信 下发设置参数给控制器
}mode_t;

typedef enum _wireless_mode
{
	RF_24L01 = 0,  //NRF
	RF_BTE = 1   //蓝牙
}wireless_mode_t;

typedef struct _wheel_
{
	pid_t	pid;
	volatile float speed;     //小车车体合成线速度值 单位[m/s]
	volatile int set;         //pid速度设置值 单位count/s
	volatile int cur_speed;   //当前速度 单位[count/s]
} wheel_t;

typedef struct _error_
{
	u64 time; /* 0:6v motor; 1:12v motor */
	int name;
} err_t;

typedef struct _robot_
{
	int num; 	/* g_robot num load from circuit configuration switch 机器号为拨码盘+1*/
	mode_t mode;	/* g_robot mode */
	wireless_mode_t rf_mode;	/* rf mode */
	u8 frq;	/* 2.4G rf freq */
	
	wheel_t wheels[ CHANNEL_NUM ]; //各个轮子的pid参数
	u16 dribbler;
	
	float kv2n;                    // 74037 m/s和count/s 转换系数 单位count/m  用于电机旋转速度转换为线速度
	int shoot;
	int chip;
	unsigned char is_ball_detected;
	int is_cap_low;
	int is_pow_low;
	int speed_x;
	int speed_y;

	vu8 cap_v;                //电容电压 单位[bit]
	vu8 bat_v;

	volatile float cap_v_f;  //充电电容电压 单位[v]
	volatile float bat_v_f;  //电池电压

	float sin_angle[ CHANNEL_NUM]; //车轮与正方向轴线的角度正弦
	float cos_angle[ CHANNEL_NUM];

	err_t error[ERROR_NUM];
	
	u16 firmware_version;    //软件版本

} robot_t;

int init_robot(void);
void do_run(void);
void SysTick_Handler(void);



extern robot_t g_robot;

#endif


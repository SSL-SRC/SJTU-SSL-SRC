/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    param.c          
* 功能描述:    
           存储器默认参数设置
* 版本      作者       时间          状态
* V1.0      shouxian   2015.09.15    创建文件
* V1.1      sunlifeng  2016.02.24    修改文件
*****************************************************************************
*****************************************************************************/ 
#include <string.h>
#include "param.h"
#include "typedef.h"
#include "cfg.h"
#include "i2c.h"


/*******************************************************************************
* Function Name   : get_param_ch_sum
* Description	  : 获取数据校验和
*					
* Input		      : param_t *param 参数结构体
* Output		  : None
* Return		  : 校验和
*******************************************************************************/
u8 get_param_ch_sum(param_t *param)
{
	u8 i = 0;
	u8 sum = 0;

	for(i = 0; i < sizeof(param->dat); i++)
	{
		sum = param->dat[i] + sum;
	}
	sum = sum + param->magic;

	return sum;
}

/*******************************************************************************
* Function Name   : init_def_param
* Description	  : 初始化默认参数
*					
* Input		      : param_t *param 参数结构体
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_def_param(param_t *param)
{
	
	memset(param, 0, sizeof(param_t));

	param->dat[0] = MOTOR_PID_KP;							//轮子1 P 参数
	param->dat[1] = MOTOR_PID_KI;							//轮子1 I 参数
	param->dat[2] = MOTOR_PID_KD;							//轮子1 D 参数

	param->dat[3] = MOTOR_PID_KP;							//轮子2 P 参数
	param->dat[4] = MOTOR_PID_KI;							//轮子2 I 参数
	param->dat[5] = MOTOR_PID_KD;							//轮子2 D 参数

	param->dat[6] = MOTOR_PID_KP;							//轮子3 P 参数
	param->dat[7] = MOTOR_PID_KI;							//轮子3 I 参数
	param->dat[8] = MOTOR_PID_KD;							//轮子3 D 参数

	param->dat[9] = MOTOR_PID_KP;							//轮子4 P 参数
	param->dat[10] = MOTOR_PID_KI;							//轮子4 I 参数
	param->dat[11] = MOTOR_PID_KD;							//轮子4 D 参数

	param->dat[12] = D_WHEEL_REDUCTION_RATIO_X;			//减速比X
	param->dat[13] = D_WHEEL_REDUCTION_RATIO_YZ; 		//减速比YZ
	param->dat[14] = MAX_SHOT_STRENGTH;					//最大射门力度		
    param->dat[15] = 0;                                 //蓝牙模块是否初始化过标志 0 未初始化过
	param->magic = MAGIC_NUM;
	
	param->chk_sum = get_param_ch_sum(param);
}


/*******************************************************************************
* Function Name   : init_param
* Description	  : 初始化默认参数
*					
* Input		      : param_t *param 参数结构体
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_param(param_t *param)
{
	
	memset(param, 0, sizeof(param_t));

	param->dat[0] = MOTOR_PID_KP;							//轮子1 P 参数
	param->dat[1] = MOTOR_PID_KI;							//轮子1 I 参数
	param->dat[2] = MOTOR_PID_KD;							//轮子1 D 参数

	param->dat[3] = MOTOR_PID_KP;							//轮子2 P 参数
	param->dat[4] = MOTOR_PID_KI;							//轮子2 I 参数
	param->dat[5] = MOTOR_PID_KD;							//轮子2 D 参数

	param->dat[6] = MOTOR_PID_KP;							//轮子3 P 参数
	param->dat[7] = MOTOR_PID_KI;							//轮子3 I 参数
	param->dat[8] = MOTOR_PID_KD;							//轮子3 D 参数

	param->dat[9] = MOTOR_PID_KP;							//轮子4 P 参数
	param->dat[10] = MOTOR_PID_KI;							//轮子4 I 参数
	param->dat[11] = MOTOR_PID_KD;							//轮子4 D 参数

	param->dat[12] = D_WHEEL_REDUCTION_RATIO_X;			//减速比X
	param->dat[13] = D_WHEEL_REDUCTION_RATIO_YZ; 		//减速比YZ
	param->dat[14] = MAX_SHOT_STRENGTH;					//最大射门力度		
    param->dat[15] = 1;                                 //蓝牙模块是否初始化过标志
	param->magic = MAGIC_NUM;
	
	param->chk_sum = get_param_ch_sum(param);
}

/*******************************************************************************
* Function Name   : load_param
* Description	  : 设置参数
*					
* Input		      : param_t *param 参数结构体
* Output		  : None
* Return		  : 0
*******************************************************************************/
int load_param(param_t *param)
{
	u8 sum;

	memset(param, 0, sizeof(param_t));

	high_size_eeprom_rd(param, 0, sizeof(param_t));

	sum = get_param_ch_sum(param);
	if((param->magic != MAGIC_NUM) || (param->chk_sum != sum))
	{
		/* error parameter data in eeprom, write default data to eeprom */
		init_def_param(param);
		high_size_eeprom_wr(param, 0, sizeof(param_t));
	}

	return 0;
}

/*******************************************************************************
* Function Name   : load_blue_param
* Description	  : 设置带蓝牙参数的函数
*					
* Input		      : None
* Output		  : None
* Return		  : 0
*******************************************************************************/
int load_blue_param(void)
{
	u8 sum;
    param_t param;
	memset(&param, 0, sizeof(param_t));

	init_param(&param);  //蓝牙模块已经初始化过
	high_size_eeprom_wr(&param, 0, sizeof(param_t));
	

	return 0;
}



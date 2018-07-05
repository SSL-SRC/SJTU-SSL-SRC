/********************************************************************************
********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:    param.c          
* ��������:    
           �洢��Ĭ�ϲ�������
* �汾      ����       ʱ��          ״̬
* V1.0      shouxian   2015.09.15    �����ļ�
* V1.1      sunlifeng  2016.02.24    �޸��ļ�
*****************************************************************************
*****************************************************************************/ 
#include <string.h>
#include "param.h"
#include "typedef.h"
#include "cfg.h"
#include "i2c.h"


/*******************************************************************************
* Function Name   : get_param_ch_sum
* Description	  : ��ȡ����У���
*					
* Input		      : param_t *param �����ṹ��
* Output		  : None
* Return		  : У���
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
* Description	  : ��ʼ��Ĭ�ϲ���
*					
* Input		      : param_t *param �����ṹ��
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_def_param(param_t *param)
{
	
	memset(param, 0, sizeof(param_t));

	param->dat[0] = MOTOR_PID_KP;							//����1 P ����
	param->dat[1] = MOTOR_PID_KI;							//����1 I ����
	param->dat[2] = MOTOR_PID_KD;							//����1 D ����

	param->dat[3] = MOTOR_PID_KP;							//����2 P ����
	param->dat[4] = MOTOR_PID_KI;							//����2 I ����
	param->dat[5] = MOTOR_PID_KD;							//����2 D ����

	param->dat[6] = MOTOR_PID_KP;							//����3 P ����
	param->dat[7] = MOTOR_PID_KI;							//����3 I ����
	param->dat[8] = MOTOR_PID_KD;							//����3 D ����

	param->dat[9] = MOTOR_PID_KP;							//����4 P ����
	param->dat[10] = MOTOR_PID_KI;							//����4 I ����
	param->dat[11] = MOTOR_PID_KD;							//����4 D ����

	param->dat[12] = D_WHEEL_REDUCTION_RATIO_X;			//���ٱ�X
	param->dat[13] = D_WHEEL_REDUCTION_RATIO_YZ; 		//���ٱ�YZ
	param->dat[14] = MAX_SHOT_STRENGTH;					//�����������		
    param->dat[15] = 0;                                 //����ģ���Ƿ��ʼ������־ 0 δ��ʼ����
	param->magic = MAGIC_NUM;
	
	param->chk_sum = get_param_ch_sum(param);
}


/*******************************************************************************
* Function Name   : init_param
* Description	  : ��ʼ��Ĭ�ϲ���
*					
* Input		      : param_t *param �����ṹ��
* Output		  : None
* Return		  : None
*******************************************************************************/
void init_param(param_t *param)
{
	
	memset(param, 0, sizeof(param_t));

	param->dat[0] = MOTOR_PID_KP;							//����1 P ����
	param->dat[1] = MOTOR_PID_KI;							//����1 I ����
	param->dat[2] = MOTOR_PID_KD;							//����1 D ����

	param->dat[3] = MOTOR_PID_KP;							//����2 P ����
	param->dat[4] = MOTOR_PID_KI;							//����2 I ����
	param->dat[5] = MOTOR_PID_KD;							//����2 D ����

	param->dat[6] = MOTOR_PID_KP;							//����3 P ����
	param->dat[7] = MOTOR_PID_KI;							//����3 I ����
	param->dat[8] = MOTOR_PID_KD;							//����3 D ����

	param->dat[9] = MOTOR_PID_KP;							//����4 P ����
	param->dat[10] = MOTOR_PID_KI;							//����4 I ����
	param->dat[11] = MOTOR_PID_KD;							//����4 D ����

	param->dat[12] = D_WHEEL_REDUCTION_RATIO_X;			//���ٱ�X
	param->dat[13] = D_WHEEL_REDUCTION_RATIO_YZ; 		//���ٱ�YZ
	param->dat[14] = MAX_SHOT_STRENGTH;					//�����������		
    param->dat[15] = 1;                                 //����ģ���Ƿ��ʼ������־
	param->magic = MAGIC_NUM;
	
	param->chk_sum = get_param_ch_sum(param);
}

/*******************************************************************************
* Function Name   : load_param
* Description	  : ���ò���
*					
* Input		      : param_t *param �����ṹ��
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
* Description	  : ���ô����������ĺ���
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

	init_param(&param);  //����ģ���Ѿ���ʼ����
	high_size_eeprom_wr(&param, 0, sizeof(param_t));
	

	return 0;
}



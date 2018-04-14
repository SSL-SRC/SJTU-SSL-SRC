/********************************************************************************
********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:	 comm.h 	   
* ��������:    
		 
* �汾		����	   ʱ�� 		 ״̬
* V1.0		shouxian   2015.9.11   �����ļ�
* V1.1		sunlifeng  2016.2.28   �޸��ļ�
*****************************************************************************
*****************************************************************************/

#ifndef __COMMM_H__
#define __COMMM_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"
#include "nrf24l01.h"

#define MAX_BUFFER_LEN 25
#define nRF2401_BUFFER_LEN 256

#define JOYSTICK_POS_LIMIT 0xa0  //ҡ��ģ�����м�������ֵ  0x80Ϊ�����м��
#define JOYSTICK_NEG_LIMIT 0x60
#define ROBOT_HIGH_SPEED_INCREMENT 0x80
#define JOYSTICK_MAX_SPEED 256  //ҡ��ҡ���׵�����ٶ� x��y���ٶȵ�λ[1cm/s] ��ת��λ[0.025rad/s]

typedef struct _comm_
{
	unsigned char buffer[MAX_BUFFER_LEN];
	unsigned char buffer_pos;

	unsigned char status;

	int packet_error; //count number of bad packet received

}rf_comm_t;

int init_comm(void);
int do_comm(void);
int do_comm_up(void);
int do_packet_process( unsigned char *data, int len );
int do_joystick_packet( unsigned char *data);

extern rf_comm_t g_rf_comm;

#endif

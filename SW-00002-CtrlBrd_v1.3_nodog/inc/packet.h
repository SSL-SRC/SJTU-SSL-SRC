/******************************************************************************
 * packet.h - 
 * 
 * Copyright 2008-2015 Shouxian Chen.
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0   2015/09/11, Shouxian Chen create this file
 * 
 ******************************************************************************/
#ifndef __PACKET_H__
#define __PACKET_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"

#define DEBUG_FUNCTION_Normal 	            0
#define DEBUG_FUNCTION_Set_9557_error       7
#define MAX_IDENTIFY_LEN                    256
#define IDENTIFY_START_ADDR                 21   //packet�е�22���ֽڱ�ʾ��֤ʶ���ַ��ʼ
typedef enum _packet_type_
{
	PACKET_NONE = -1,
	PACKET_Normal = 0,
	PACKET_DEBUG_Check_Frq = 2,
	PACKET_DEBUG_Set_9557_error = 7
} packet_type_t;

typedef struct _packet_robot_
{
	int robot_num; //robot id
	
	int dribbler;  //����λ
	int shoot;     //ƽ��
	int chip;      //����
	int speed_x;   //С������x�᷽���ٶ� ��λ[cm/s] X��Ϊ������ͷ���
	int speed_y;   //y�᷽���ٶ� ��λ[cm/s]
	int speed_rot; //��ת�����ٶ�  ��λ[1��ʾ0.25rad/s]
	
} packet_robot_t;

/**cpuid���ջ���һ�����ܵ�cpuidΪ8Byte ���֧��32��robot ͬʱ����cpuid��֤**/
typedef struct
{
    unsigned char   recv_cpuid[MAX_IDENTIFY_LEN];
	unsigned short  recv_packet_cnt;              //����robot����֤cpuid ����Ҫ����İ�����
	unsigned char   recv_cpuid_start_flag;        //��ʼ����cpuid���ݱ�־
	unsigned char   recv_cpuid_ok;               //cpuid��֤��ȫ��������ϱ�־
}idenfity_cpuid_struct;

packet_type_t get_packet_type( unsigned char* data, int len );
int stop_mode_packet(char *q);
int packet(char *q);
int decode_packet_robot( packet_robot_t *packet, unsigned char *data, int len );
int decode_identify_packet( idenfity_cpuid_struct *id_code, unsigned char *data );


#endif

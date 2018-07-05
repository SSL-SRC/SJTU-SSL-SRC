/******************************************************************************
 * packet.c - 
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
#include "packet.h"
#include "robot.h"
#include "misc.h"
#include "timer.h"
#include <string.h>

char packet_flag;

extern int max_shot_strength_set;

extern char shooter;

extern timer_t rf_comm_tim;

extern timer_t identify_cpuid_tim;

extern u8  encrpty_cpuid[8];

unsigned int identify_buf_ptr;

unsigned char identify_success = 1;   //��֤�ɹ���־λ 1 ��֤�ɹ� 0 ��֤ʧ�� ��ʼֵΪ1 ����������10S�ٽ�����֤

extern int do_power_monitor(void);

/******************************************************************************
 * stop_mode_packet: 
 *		���յ�stop��־λ��������һ��Э��Ļذ�,�μ�Э��0.3������ͣʱ��֡���ذ���ʽ
 *
 * Parameter:
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *    2015/09/11, Shouxian Chen create this function
 * 
 ******************************************************************************/
int stop_mode_packet(char *q)
{
	u8 bat_v;
	u8 cap_v;
	
	q[0] = 0xff;
	q[1] = 0x01;
	q[2] = g_robot.num & 0x0f;
	
	bat_v = get_bat_v();
	cap_v = get_cap_v();

	DIS_INT();
	g_robot.bat_v = bat_v;
	g_robot.cap_v = cap_v;
	
	if(g_robot.bat_v == 0xFF)
	{
		g_robot.bat_v = 0xFE;
	}
	q[3] = g_robot.bat_v;
	
	if(g_robot.cap_v == 0xFF)
	{
		g_robot.cap_v  = 0xFE;
	}
	q[4] = g_robot.cap_v ;
	
	g_robot.bat_v_f = g_robot.bat_v * BAT_V_ADC_GAIN;
	g_robot.cap_v_f = g_robot.cap_v * CAP_V_ADC_GAIN;

	EN_INT();
	
	q[5] = (is_infra_broken() << 7);	//����ֻ���˺����־һλ����Ļ��д����
	packet_flag = 1;
	
	return 0;
}


/******************************************************************************
 * packet: 
 *		��������������践����λ������Ϣ�����������Ƿ���״̬�仯(ƽ�䡢���䡢����
 *	��һ�ֱ仯)��������״̬�仯ʱ����λ�ϴ���־ packet_flag.
 *	�μ�Э��0.2����ʱ��֡���ذ���ʽ.
 *
 * Parameter:
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *    2015/09/11, Shouxian Chen create this function
 * 
 ******************************************************************************/
int packet(char *q)
{
	static int last_infra = 0;
	static int now_infra = 0;

	static int to_shoot = 0;
	static int to_chip = 0;

	static int finish_shoot = 0;
	static int finish_chip = 0;

	static int m = 0;
	static int n = 5;

    now_infra = is_ball_detected();

	/* ���ȣ���������������ύ������to_shoot */
	if(shooter == 0x02) //shoot
	{
	    to_shoot = 1;
		shooter = 0x00;
	}
        
	if(shooter == 0x01)//chip
	{
		to_chip = 1;
    	shooter = 0x00;
	}
	
    if(now_infra == 0)
    {
		/* �����������û�򣬵�shoot�����У�˵�����Ѿ��߳�ȥ�� */
        if(to_shoot == 1)
        {
            finish_shoot = 1;
            to_shoot = 0;
			n = 0;
			m++;
			packet_flag = 1;
        }
		
        if(to_chip == 1)
        {
            finish_chip = 1;
            to_chip = 0;
			n = 0;
			m++;
			packet_flag = 1;
        }    
		
		/* ENABLE_INFRA_BEEP������� */
		#ifdef ENABLE_INFRA_BEEP
			if(do_power_monitor() == 0)	BEEP_OFF();
		#endif
	}
	else  //��ס���������
	{
		#ifdef ENABLE_INFRA_BEEP
			BEEP_ON();
		#endif
	}

	/* n��¼ÿ��packet����ʱ�Ĵ���, ÿ���·��İ�ִ��5�� */
	if(n >= 5)   	
	{
		if(finish_shoot == 1) finish_shoot = 0;
		if(finish_chip == 1) finish_chip = 0;
		
		if((last_infra != now_infra))
		{
			n = 1;
			m++;
			packet_flag = 1;
		}
		else
		{
			packet_flag = 0;
		}
	}			
	else
	{
		n++;
		packet_flag = 1;
	}

	if(m == 127)
	{	
		m = 0;
	}

	q[0] = 0xff;
    q[1] = 0x02;
    q[2] = g_robot.num & 0x0F;
	q[3] = (now_infra << 6) + (finish_shoot << 5) + (finish_chip << 4);
	q[4] = m;
	q[5] = n;

	last_infra = now_infra;

	return 0;
}

 /*******************************************************************************
* Function Name   : get_packet_type
* Description	  : ��ȡ����ģʽ������֮����
*					
* Input		      : unsigned char* data, int len 
* Output		  : None
* Return		  : packet_type_t
*******************************************************************************/
packet_type_t get_packet_type( unsigned char* data, int len )
{
	/* check run-time mode */
	volatile unsigned char temp;
	temp = data[1];
	temp = temp & 0xF0;
	temp = temp >> 4;
	
	switch(temp)
	{
		case DEBUG_FUNCTION_Normal:                  	//����ģʽ
			return PACKET_Normal;

		case DEBUG_FUNCTION_Set_9557_error:
			return PACKET_DEBUG_Set_9557_error; 

		default:
			return PACKET_NONE;                    	//�������ݰ�
	}
}


/*******************************************************************************
* Function Name   : decode_packet_robot
* Description	  : ���������Ӧ���ŵ�С�������˶�����
*					
* Input		      : packet_robot_t *packet, unsigned char *data, int len
* Output		  : None
* Return		  : 0
*******************************************************************************/
int decode_packet_robot( packet_robot_t *packet, unsigned char *data, int len )
{
	unsigned short temp = 0;
	unsigned char  i=0;  
	u8 pos; 
	unsigned short high_value_x;
	unsigned short high_value_y;
	unsigned short high_value_r;
  
	if(packet == NULL || data == NULL)
		return -1; 

	/* ������Ŵ���8����packet�г��ŷ���data[1]�ĵ�4λ��������data[2]�� */
    if(g_robot.num > 8)
    {
	    if( ( (data[1] & 0x0f) & (0x01 << (g_robot.num - 9)) ) == 0 )
	    {
		    return  -1;  		//�·����ݰ�û���Լ��ĳ��������򲻽��ܸ����ݰ�
	    }
    }
    else
    {
		/* �ж��������Ƿ����Լ������� */
 	    if( ((data[2] & 0xff) & (0x01 << (g_robot.num - 1)) ) == 0 )  
	    { 
	      	return  -1;  	
	    }
    }

	/* �յ��Լ������ݣ�ͨѶ������㡣*/
	rf_comm_tim = get_one_timer(COMM_TIMEOUT_TIME);


  	/* ����packet�У�һ���������ٸ��������ݣ����ҵ��Լ������ݵ�λ�� */
	pos = 0;
    if(g_robot.num < 9) //ROBOT1~8
    {
	    for(i = 0; i < (g_robot.num - 1); i++)
	  	{
	  	    if(data[2] & (0x01 << i))
		        pos++; //�ҳ�  
		 }
    }
    else  //robot9~12
    {
 	    for(i = 0; i < 8; i++)
	  	{
	  	 	if(data[2] & (0x01 << i))
		 	pos++;	
		}
        for(i = 0; i < (g_robot.num - 9); i++)
    	{
        	if((data[1] & 0x0f) & (0x01 << i))
        	pos++;
     	}
  	}
  	i = pos * 4 + 3; //������ʼ��
	
   	packet->robot_num = g_robot.num;

	/* set robot value from packet data */
	temp = data[i];
	packet->dribbler = ((( temp >> 4 ) & 0x03));	
	packet->dribbler = (( temp & 0x80) ? (-packet->dribbler) : packet->dribbler);
	//temp = data[i+15]; //��������
	temp = data[pos+18]; //��������
	
	if( (data[i] >> 6) & 0x1 ) //����
	{
		/* chip */
    	if(temp >= 127 )
			packet->chip = MAX_SHOT_STRENGTH;
	    else
			packet->chip= temp;

	}
	else //ƽ��
	{   
		/* shoot */
  
		
		if(temp >= 127 )
			packet->shoot = MAX_SHOT_STRENGTH;

		else
			packet->shoot = temp;
	}					


	
	temp = data[i+1]; //Speed_x
	packet->speed_x = temp & 0x7F;
	high_value_x = (unsigned short)data[pos + 15];
	high_value_x = ((unsigned short)(high_value_x & 0xc0)) << 1;
	packet->speed_x = packet->speed_x + high_value_x; //�ٶ�ֵ+max(0x80) 127+128=256
	temp = data[i+1];
	packet->speed_x = ( ( temp & 0x80 ) ? ( -packet->speed_x ) : packet->speed_x );
		 
	temp = data[i+2]; //speed_y
	packet->speed_y = temp & 0x7F;
	high_value_y = (unsigned short)data[pos + 15];
	high_value_y = ((unsigned short)(high_value_y & 0x30)) << 3;
	packet->speed_y = packet->speed_y+high_value_y;
	temp = data[i+2];
	packet->speed_y = ( ( temp & 0x80 ) ? ( -packet->speed_y ) : packet->speed_y );
		 
	temp = data[i+3]; //speed_rote
	packet->speed_rot = temp & 0x7F;
	high_value_r = (unsigned short)data[pos + 15];
 	high_value_r = ((unsigned short)(high_value_r & 0x0c)) << 5;
	packet->speed_rot = packet->speed_rot+high_value_r;
	temp = data[i+3];
	packet->speed_rot = ( ( temp & 0x80 ) ? ( -packet->speed_rot ) : packet->speed_rot );

	return 0;
}

/*******************************************************************************
* Function Name   : compare_data
* Description	  : ���ݱȽ�
*					
* Input		      : u8 data[],u8 data1[],int len 
* Output		  : None
* Return		  : ����1��ʾ�����2������Ԫ��ֵ��ͬ 0��ʾ��ͬ
*******************************************************************************/
int compare_data(u8 data[],u8 data1[],int len )
{
     int i;

	 if(data == NULL || data1 == NULL)
		return -1; 
	 for( i = 0; i < len ; i ++)
	 {
	   if(data[i] != data1[i])
	   {
	       return 0; 
	   }
	   
	 }

	 return 1;
	 
}

/*******************************************************************************
* Function Name   : cpuid_identify
* Description	  : cpuid��֤��ʶ��
*					
* Input		      : idenfity_cpuid_struct *id_code, unsigned char *data, int len
* Output		  : None
* Return		  : ����1��ʾCPUID��֤��ȷ ����0��ʾ��֤ʧ��
*******************************************************************************/
int cpuid_identify( idenfity_cpuid_struct *id_code )
{
    int i;
	u8 *cpuid_p;
	u8 rtn = 0;
	cpuid_p = &id_code->recv_cpuid[0];

	for(i = 0; i < (MAX_IDENTIFY_LEN/8) ; i ++) //���֧��32��robot��id��֤
	{
	 
	   rtn = compare_data(&encrpty_cpuid[0],cpuid_p,8);
	   if(rtn == 0) //��8��Byte�����cpuid����ͬ
	   {
	      cpuid_p += 8; 
	   }
	   else if(rtn == 1)//�ҵ���ͬ��ID����֤�ɹ�
	   {
	       return rtn;
	   }
	}
	
	return rtn; //ID �����δ�ҵ����ϵ�ID
	 

}


/*******************************************************************************
* Function Name   : decode_identify_packet
* Description	  : ��ѹcpuid��֤��
*					���С�ͻ����˿�������֤�ĵ�
* Input		      : idenfity_cpuid_struct *id_code, unsigned char *data, int len
* Output		  : None
* Return		  : 
*******************************************************************************/
int decode_identify_packet( idenfity_cpuid_struct *id_code, unsigned char *data )
{
	
	char i;
    static short identify_packet_cnt = 0;
  
	if(id_code == NULL || data == NULL)
		return -1; 

	/*data[21]��ʾcpuid��֤����*/
    if(data[IDENTIFY_START_ADDR] & 0x80) //���λΪ1���ʾ�����֤���ݰ���ʼ
    {  
       identify_buf_ptr = 0;
       identify_packet_cnt = 0;
       id_code->recv_packet_cnt = (data[IDENTIFY_START_ADDR] & 0x7f) + 1;  //��֤����Ҫ����packet����
       id_code->recv_cpuid_start_flag = 1;
	   
    }
	if(id_code->recv_cpuid_start_flag)
	{   
	    identify_packet_cnt++;  //
	    for(i = 0; i < 2; i ++)
	    {
	        id_code->recv_cpuid[identify_buf_ptr++] = data[IDENTIFY_START_ADDR+1+i];
	    }
		if(identify_packet_cnt == id_code->recv_packet_cnt)//��������֤���ݰ��������֤
		{
		  id_code->recv_cpuid_ok = 1;//��֤���ݰ��������
		  id_code->recv_cpuid_start_flag = 0;
		  identify_packet_cnt = 0;
		}
		
	      
	}
	if( id_code->recv_cpuid_ok == 1)
	{
	    if(cpuid_identify(id_code) == 1)//��֤�ɹ�
	    {
	          /* ���ʶ��ɹ����ӳ�ʱ��������¼�ʱ10s*/
	         identify_cpuid_tim = get_one_timer(IDENTIFY_CPUID_TIMEOUT_TIME);//10s
	         identify_success = 1;
			 
	    }
		else  //��֤ʧ�� CPUIDδע���
		{
		     identify_success = 0;
			
		}
	     memset(id_code , 0, sizeof(idenfity_cpuid_struct));
	}
	if(identify_buf_ptr >= (MAX_IDENTIFY_LEN - 1))
	{
	    identify_buf_ptr = 0;
	}



	return 0;
}



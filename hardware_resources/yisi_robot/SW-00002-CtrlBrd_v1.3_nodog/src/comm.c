 /********************************************************************************
 ********************************************************************************
 * Ȩ��:	�����Ͻ������˹ɷ����޹�˾
 * �ļ���:	  comm.c 		
 * ��������:	
		    ����ͨ�Ų������ݴ���
 * �汾 	 ����		ʱ��		  ״̬
 * V1.0 	 shouxian	2015.9.11	�����ļ�
 * V1.1      sunlifeng  2016.2.28   �޸��ļ�
 *****************************************************************************
 *****************************************************************************/
#include <string.h>
#include "packet.h"
#include "robot.h"
#include "misc.h"
#include "timer.h"
#include "comm.h"
#include "action.h"
#include "usart_ble.h"
#include "pid.h"
#include "math.h"
rf_comm_t g_rf_comm;

packet_type_t type;                         //���ݰ�����
unsigned char packet_cnt=0;                 //�հ���������  

extern char packet_flag;
extern nRF24L01 nRF24L01_dev;
extern char g_do_set_receive_mode_flag;

extern int max_shot_strength_set;
extern int hm_bluetooth_connect_status(void);
packet_robot_t joystick_packet;
idenfity_cpuid_struct identify_data = {0};
char gyro_control_flag = 0 ;  //�ñ�־Ϊ1 �ſ��������Ǳջ�����

/*******************************************************************************
* Function Name   : init_comm
* Description	  : ����ͨ�Ų��ֳ�ʼ��
*	                ͨ��ģ���ʼ������,��ʼ�������Ƚ�ͨ��ģ���ʼ��Ϊ����ģʽ.				
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int init_comm(void)
{
	nRF24L01_dev.buf.len = nRF2401_BUFFER_LEN;
	nRF24L01_dev.buf.pos = 0;
	nRF24L01_dev.get_packet = get_nRF24L01_packet;
	nRF24L01_dev.send_packet = send_nRF24L01_packet;
	nRF24L01_dev.init_dev = nrf24l01_init;
	nRF24L01_dev.packet_error = 0;

	nrf24l01_init();
	
	start_nRF24L01_RX();
	set_receive_flag();
	
	return 0;
}


/*******************************************************************************
* Function Name   : do_comm
* Description	  : ͨѶ���ܺ���,�����ж��Ƿ��յ�ͨѶ��,����յ�,��ͨѶ��������g_rf_comm.buffer��
*	                ���н��,���ϴ�һ������			
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_comm(void)
{
	 
	if(get_receive_flag())//����ģʽ
	{   
	    if(g_robot.rf_mode == RF_24L01) //������Ƶͨ��ģʽ
	    {   
			if( ( nRF24L01_dev.get_packet( &nRF24L01_dev ) ) > 0 ) //���յ�һ�����ݰ�
			{
				clr_receive_flag();
				memcpy( g_rf_comm.buffer, nRF24L01_dev.buf.buf, nRF24L01_dev.buf.pos );
				g_rf_comm.buffer_pos = (unsigned char)nRF24L01_dev.buf.pos;
	                              
				do_packet_process(g_rf_comm.buffer, g_rf_comm.buffer_pos); 

				if(packet_cnt > 254) packet_cnt = 0;
				else packet_cnt++;
			}
	    }
		else if(g_robot.rf_mode == RF_BTE) //����ͨ��ģʽ
		{   
		   if(hm_bluetooth_connect_status() == 1) //�������ӳɹ�
		   {
		      if(get_uart_ble_packet(&buf_packet_ok[0]) == 0)
		      {
						
			      memcpy( g_rf_comm.buffer, &buf_packet_ok[0], UART_PCKT_RX_SIZE );
				    g_rf_comm.buffer_pos = UART_PCKT_RX_SIZE;
		       				
					  do_packet_process( g_rf_comm.buffer, g_rf_comm.buffer_pos ); 
			      g_is_uart_get_a_packet = 0;
			  }
		   }
		  
           
		}
	}
	return 0;
}

/*******************************************************************************
* Function Name   : do_comm_up
* Description	  : �ϴ�ִ�к���������ȫ�ֱ���type����ֵ���������ú���ģʽ�����ϴ�
*	              		
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_comm_up(void)
{
	short temp;
	int i;
	static char data[7]={0,0,0,0,0,0,0}; 

	nRF24L01_dev.buf.pos = PACKET_LEN_UP;

	/*----------------------------ͨ��ģʽ�µ������ϴ�--------------------------------*/

	if(type == PACKET_Normal)
	{
		/* ����ʱ�����ϴ� */
		/* �μ�Э��3.0������ͣʱ��֡���ذ���ʽ */

		/* �ж�stop��־ test-lnc */
		if( (((g_rf_comm.buffer[3] & 0x8) >> 3) == 1) || (((g_rf_comm.buffer[7] & 0x8) >> 3) == 1) 
			|| (((g_rf_comm.buffer[11] & 0x8) >> 3) == 1)) //bit3 Ϊ1 ���ϴ�ģʽΪ2�ϴ���ص��ݵ�ѹ
		{
			temp=0;
            packet_flag=0;
            
			if(g_robot.num > 8)
			{
				if(((g_rf_comm.buffer[1] & 0x0f) & (0x01 << (g_robot.num - 9))) == 0)
				{   
				  temp = -1;   //�·����ݰ�robot ID����Ƴ�ID��һ��
				}
			}
			else
			{
				/* �жϰ��������������Ƿ����Լ��� */
				if(((g_rf_comm.buffer[2] & 0xff ) & (0x01 << (g_robot.num - 1))) == 0)	
				{ 
				  temp = -1;  //�·����ݰ�robot ID����Ƴ�ID��һ��
				}
			}
		
			if(temp != -1)  //�·����ݰ��복����IDһ��
			{
				/* �����Լ����İ� */
				/* ���Լ��ĳ��������������� */
                temp = 0;
		      	if(g_robot.num < 9)
			    {
				    for(i = 0; i < (g_robot.num - 1); i++)
					{
						if(g_rf_comm.buffer[2] & (0x01 <<i )) temp++;
					}
			    }
                else
			    {
			        for(i = 0; i < 8; i++)
				    {
				        if(g_rf_comm.buffer[2] & (0x01 << i)) temp++; 
				    }
				    for(i = 0; i < (g_robot.num - 9); i++)
				    {
				        if((g_rf_comm.buffer[1] & 0x0f) & (0x01 << i)) temp++;
			        }
                }
			}
		
			if( (temp != -1) && (((g_rf_comm.buffer[3+4*temp] & 0x8) >> 3) == 1) )
			{
				stop_mode_packet(data);

				if(packet_flag == 1)
				{   
					nRF24L01_dev.buf.buf[0]=data[0];
					nRF24L01_dev.buf.buf[1]=data[1];
					nRF24L01_dev.buf.buf[2]=data[2]; 
					nRF24L01_dev.buf.buf[3]=data[3];
					nRF24L01_dev.buf.buf[4]=data[4];
					nRF24L01_dev.buf.buf[5]=data[5]; 
					memset(&nRF24L01_dev.buf.buf[6], 0, sizeof(unsigned char) * (PACKET_LEN_UP - 6));
										  
					nRF24L01_dev.send_packet( &nRF24L01_dev );

					g_do_set_receive_mode_flag = 1;
				}
			}
		}
		else    //bit3 Ϊ0 ���ϴ����� ƽ����Ϊ��ģʽ
		{
			/*--------------�μ�Э��0.2����ʱ��֡���ذ���ʽ----------------------*/
			/* ����packet()����ϴ����� */
			packet(data);
			
		    if(packet_flag == 1)
			{
				nRF24L01_dev.buf.buf[0]=0xFF;	            
				nRF24L01_dev.buf.buf[1]=0x02;	                
				nRF24L01_dev.buf.buf[2]=data[2]; 
				nRF24L01_dev.buf.buf[3]=data[3];
				nRF24L01_dev.buf.buf[4]=data[4];
				nRF24L01_dev.buf.buf[5]=data[5];
				memset(&nRF24L01_dev.buf.buf[6], 0, sizeof(unsigned char) * (PACKET_LEN_UP - 6));			
				nRF24L01_dev.send_packet( &nRF24L01_dev );
			}
		}
	}
	
	g_do_set_receive_mode_flag = 1;
	
	return 0;
}

/*******************************************************************************
* Function Name   : do_packet_process
* Description	  : �Խ��յ�������data���н�������ݲ�ͬ��ģʽ�в�ͬ�Ľ����ʽ����Ӧ�в�ͬ��
*	              	�������,����Ľ��������λ��Packet.c��
* Input		      : unsigned char *data, int len 
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_packet_process( unsigned char *data, int len )
{   
	/* determine the packet type and parse it */
    static int temp = 0;
	
    if(temp == 0)
    {
        type = get_packet_type( data, len );//��ȡ���ݰ�ģʽ
        if(type == PACKET_Normal) //����ģʽ
        { 
            temp = 1;
        }
    }

	do_comm_up();
	
  	decode_identify_packet( &identify_data, data );//����ÿ���յ���cpuid��֤�� ���߽��յ����κΰ���Ҫ�������ݻ�ȡ����cpuid��֤
    /*----------------------------ͨ��ģʽ�µ����ݽ��--------------------------------*/
	/*-----------------------�μ�Э��0.1����ʱͨѶ����ʽ-----------------------------*/
	if((type == PACKET_Normal) && (g_robot.mode == NORMAL_MODE))
	{
		/* parse robot command */
		packet_robot_t packet;
    
		memset( &packet, 0, sizeof( packet ) ); //ÿ������ �·��ٶ�ֵ����0

		if( decode_packet_robot( &packet, data, len ) < 0 )
		{
			/* parse error */
			g_rf_comm.packet_error++;
			return -1;
		}
		
		
		/* execute the command */
		on_robot_command( &packet );
	}
	
	return 0;
}

/*******************************************************************************
* Function Name   : do_joystick_packet
* Description	  : ��ѹ�ֱ����ݰ� ���������ǵĿ����������С��ƽ��
*	              	
* Input		      : unsigned char *data
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_joystick_packet( unsigned char *data)
{   
		/* parse robot command */
		float x_k1,x_b1,x_k2,x_b2,r_k1,r_b1,r_k2,r_b2,y_k1,y_b1,y_k2,y_b2;
		unsigned char shoot_strength; //ģ����
		unsigned char button_key;
        unsigned char ctl_dibbler;//����λ
        static int shoot_delay_count = 0;
		static int chip_delay_count = 0;
		static vu64  shoot_current_time = 0;
        vu64 shoot_delay_time = 0;
		static vu64  chip_current_time = 0;
        vu64 chip_delay_time = 0;
		char rot_joystick =0;
		char move_joystick_x = 0;
		char move_joystick_y = 0;
		
		button_key = data[0];
		shoot_strength = data[17];//�������� LT���� 0~255
		if(shoot_strength < 3)
	    {
	      shoot_strength = 0;
	    }
		else
		{
		  shoot_strength = shoot_strength/2 + 3; //��������0~127 
		}
		
		ctl_dibbler = data[18];//����λģ����  0~64 0�� 64~128 1�� 128~192 2�� 192~256 3��
		
		x_k1 = ((float)(-JOYSTICK_MAX_SPEED-0)/(float)(0xff-JOYSTICK_POS_LIMIT));
		x_b1 = 0-x_k1*JOYSTICK_POS_LIMIT;
		x_k2 = ((float)(JOYSTICK_MAX_SPEED-0)/(float)(0x00-JOYSTICK_NEG_LIMIT));
		x_b2 = 0-x_k2*JOYSTICK_NEG_LIMIT;

		y_k1 = ((float)(JOYSTICK_MAX_SPEED-0)/(float)(0xff-JOYSTICK_POS_LIMIT));
		y_b1 = 0-y_k1*JOYSTICK_POS_LIMIT;
		y_k2 = ((float)(-JOYSTICK_MAX_SPEED-0)/(float)(0x00-JOYSTICK_NEG_LIMIT));
		y_b2 = 0-y_k2*JOYSTICK_NEG_LIMIT;

		r_k1 = ((float)(JOYSTICK_MAX_SPEED-0)/(float)(0xff-JOYSTICK_POS_LIMIT));
		r_b1 = 0-r_k1*JOYSTICK_POS_LIMIT;
		r_k2 = ((float)(-JOYSTICK_MAX_SPEED-0)/(float)(0x00-JOYSTICK_NEG_LIMIT));
		r_b2 = 0-r_k2*JOYSTICK_NEG_LIMIT;
        if((data[3]<=JOYSTICK_POS_LIMIT) && (data[3]>=JOYSTICK_NEG_LIMIT))
        {
          // data[3] = 0x80;
		   joystick_packet.speed_y = 0;
		   move_joystick_y = 0;
        }
		else
		{
		   move_joystick_y = 1;//ˮƽ�����˶�ҡ�˱��ٿ�
		}
		if((data[4]<=JOYSTICK_POS_LIMIT) && (data[4]>=JOYSTICK_NEG_LIMIT))
        {
          // data[4] = 0x80;
          joystick_packet.speed_x = 0;
		  move_joystick_x = 0;
          
        }
		else
		{
		   move_joystick_x = 1; //ǰ����ҡ�˱��ٿ�
		}
		if((data[5]<=JOYSTICK_POS_LIMIT) && (data[5]>=JOYSTICK_NEG_LIMIT))
        {
          // data[4] = 0x80;
          joystick_packet.speed_rot = 0;  //����С����תҡ��δ�ٿض��ҿ���С��ǰ����ҡ�˲ٿ�������������Ǳջ�����
		  rot_joystick  = 1;  //�ֱ�����С����תҡ��δ�ٿ�����������1
          
        }
		else//��תҡ�˲ٿ����������Ǳջ�����ȡ��
		{
		   rot_joystick = 0; 
		}
		/***��ͨ���Ͻ�ģ����ҡ�˿���XY����***/
		if(data[4]>=JOYSTICK_POS_LIMIT)//�ֱ�ҡ��Y���൱��С��X�� ǰ���� ҡ��Y����0x00 ��0xff 
		{
		   joystick_packet.speed_x = (int)((float)(x_k1*data[4])+x_b1);  //��С��X������˶�
		}
		else if(data[4]<=JOYSTICK_NEG_LIMIT)//ҡ������ С������X����ǰ�˶�
		{
		  joystick_packet.speed_x = (int)((float)(x_k2*data[4])+x_b2);
		}

		if(data[3]>=JOYSTICK_POS_LIMIT) //С��ˮƽ������
		{
		   joystick_packet.speed_y = (int)((float)(y_k1*data[3])+y_b1);
		}
		else if(data[3]<=JOYSTICK_NEG_LIMIT) //С��ˮƽ������
		{
		  joystick_packet.speed_y = (int)((float)(y_k2*data[3])+y_b2);
		}	
		//joystick_packet.speed_x = -(data[4]-0x80);
		//joystick_packet.speed_y = (data[3]-0x80);
        /***��ͨ���½�ģ����ҡ�˿�����ת***/

		if(data[5]>=JOYSTICK_POS_LIMIT) //��ȡС����ת�ٶ�
		{
		   joystick_packet.speed_rot = (int)((float)(r_k1*data[5])+r_b1);
		}
		else if(data[5]<=JOYSTICK_NEG_LIMIT)
		{
		  joystick_packet.speed_rot = (int)((float)(r_k2*data[5])+r_b2);
		}

	    for(;;)
	    {
			if(button_key &0x01) //X������ ƽ��
			{
			  /* shoot */
			   shoot_delay_count++;
			   if(shoot_delay_count == 1)
			   {
				    shoot_current_time = get_sys_tick();
	    	        if(shoot_strength >= 127 ) //LTģ���� ��������
				      joystick_packet.shoot = MAX_SHOT_STRENGTH;
		            else
				      joystick_packet.shoot= shoot_strength;

					  break;
			   }
			   shoot_delay_time = get_sys_tick();
			   if((shoot_delay_time - shoot_current_time) > 2500)
			   {
			     	 shoot_delay_count = 0;
					 if(shoot_strength >= 127 ) //LTģ���� ��������
				      joystick_packet.shoot = MAX_SHOT_STRENGTH;
		             else
				      joystick_packet.shoot= shoot_strength;

					 break;
				 
			   }
			   else
			   {
			      joystick_packet.shoot = 0;
				  break;
			   }
	           
			}
			else if((button_key >> 2) & 0x01)//B �������� ����
				{  
				   chip_delay_count++;
				   if(chip_delay_count == 1)
				   {
				     /* chip */
					  chip_current_time = get_sys_tick();
	    	         if(shoot_strength >=127 ) //LTģ���� ��������
				      joystick_packet.chip = MAX_SHOT_STRENGTH;
		             else
				      joystick_packet.chip= shoot_strength;
					 
					 break;
				   }
				   chip_delay_time = get_sys_tick();
			       if((chip_delay_time - chip_current_time) > 2500)
			       {
			         chip_delay_count = 0;
					 if(shoot_strength >=127 ) //LTģ���� ��������
				      joystick_packet.chip = MAX_SHOT_STRENGTH;
		             else
				      joystick_packet.chip= shoot_strength;

					 break;
			       }
				   else
			       {
			          joystick_packet.chip = 0;
				      break;
			       }
				   
				   
				   
				}
			    else //ƽ�����䶼û����������������0
			    {
			      joystick_packet.shoot = 0;
				  joystick_packet.chip = 0;
				  break;
			    }
	    }

		
		/**����λ**/
		if(ctl_dibbler < 64)
		{
		   joystick_packet.dribbler = 0;
		}
		else if((ctl_dibbler >= 64) && (ctl_dibbler < 128))
		    {
		      joystick_packet.dribbler = 1;
		    }
		    else if((ctl_dibbler >= 128) && (ctl_dibbler < 192))
			    {
			      joystick_packet.dribbler = 2;
			    }
		        else if(ctl_dibbler >= 192)
		    	    {
		    	      joystick_packet.dribbler = 3;
		    	    }

    
        if((rot_joystick == 1)&&((move_joystick_x == 1)||(move_joystick_y == 1)))
        {
            gyro_control_flag = 1;  //����С��ǰ�����˵�ҡ���ƶ��ҿ���С����ת��ҡ��δ�ƶ�����������Ǳջ�����
            joystick_packet.speed_rot = (int)(gyro_pid.out);
        }
		else
		{
		    gyro_control_flag = 0;
		}
		
		
		
		/* execute the command */
		do_robot_command( &joystick_packet );
	
	
	return 0;
}


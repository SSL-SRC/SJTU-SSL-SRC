 /********************************************************************************
 ********************************************************************************
 * 权限:	杭州南江机器人股份有限公司
 * 文件名:	  comm.c 		
 * 功能描述:	
		    无线通信部分数据处理
 * 版本 	 作者		时间		  状态
 * V1.0 	 shouxian	2015.9.11	创建文件
 * V1.1      sunlifeng  2016.2.28   修改文件
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

packet_type_t type;                         //数据包类型
unsigned char packet_cnt=0;                 //收包数计数器  

extern char packet_flag;
extern nRF24L01 nRF24L01_dev;
extern char g_do_set_receive_mode_flag;

extern int max_shot_strength_set;
extern int hm_bluetooth_connect_status(void);
packet_robot_t joystick_packet;
idenfity_cpuid_struct identify_data = {0};
char gyro_control_flag = 0 ;  //该标志为1 才开启陀螺仪闭环控制

/*******************************************************************************
* Function Name   : init_comm
* Description	  : 无线通信部分初始化
*	                通信模块初始化函数,初始化中首先将通信模块初始化为接受模式.				
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
* Description	  : 通讯接受函数,首先判定是否收到通讯包,如果收到,将通讯包拷贝至g_rf_comm.buffer中
*	                进行解包,并上传一次数据			
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_comm(void)
{
	 
	if(get_receive_flag())//接收模式
	{   
	    if(g_robot.rf_mode == RF_24L01) //无线射频通信模式
	    {   
			if( ( nRF24L01_dev.get_packet( &nRF24L01_dev ) ) > 0 ) //接收到一个数据包
			{
				clr_receive_flag();
				memcpy( g_rf_comm.buffer, nRF24L01_dev.buf.buf, nRF24L01_dev.buf.pos );
				g_rf_comm.buffer_pos = (unsigned char)nRF24L01_dev.buf.pos;
	                              
				do_packet_process(g_rf_comm.buffer, g_rf_comm.buffer_pos); 

				if(packet_cnt > 254) packet_cnt = 0;
				else packet_cnt++;
			}
	    }
		else if(g_robot.rf_mode == RF_BTE) //蓝牙通信模式
		{   
		   if(hm_bluetooth_connect_status() == 1) //蓝牙连接成功
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
* Description	  : 上传执行函数，根据全局变量type的数值来决定运用何种模式进行上传
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

	/*----------------------------通常模式下的数据上传--------------------------------*/

	if(type == PACKET_Normal)
	{
		/* 比赛时数据上传 */
		/* 参见协议3.0比赛暂停时分帧返回包格式 */

		/* 判断stop标志 test-lnc */
		if( (((g_rf_comm.buffer[3] & 0x8) >> 3) == 1) || (((g_rf_comm.buffer[7] & 0x8) >> 3) == 1) 
			|| (((g_rf_comm.buffer[11] & 0x8) >> 3) == 1)) //bit3 为1 则上传模式为2上传电池电容电压
		{
			temp=0;
            packet_flag=0;
            
			if(g_robot.num > 8)
			{
				if(((g_rf_comm.buffer[1] & 0x0f) & (0x01 << (g_robot.num - 9))) == 0)
				{   
				  temp = -1;   //下发数据包robot ID与控制车ID不一致
				}
			}
			else
			{
				/* 判断包中三个车号里是否有自己的 */
				if(((g_rf_comm.buffer[2] & 0xff ) & (0x01 << (g_robot.num - 1))) == 0)	
				{ 
				  temp = -1;  //下发数据包robot ID与控制车ID不一致
				}
			}
		
			if(temp != -1)  //下发数据包与车本身ID一致
			{
				/* 具有自己车的包 */
				/* 将自己的车号在三辆中排序 */
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
		else    //bit3 为0 则上传红外 平常就为该模式
		{
			/*--------------参见协议0.2比赛时分帧返回包格式----------------------*/
			/* 调用packet()打包上传数据 */
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
* Description	  : 对接收到的数据data进行解包，根据不同的模式有不同的解包方式，对应有不同的
*	              	解包函数,具体的解包函数均位于Packet.c中
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
        type = get_packet_type( data, len );//获取数据包模式
        if(type == PACKET_Normal) //比赛模式
        { 
            temp = 1;
        }
    }

	do_comm_up();
	
  	decode_identify_packet( &identify_data, data );//处理每包收到的cpuid认证码 无线接收到的任何包都要进行数据获取进行cpuid认证
    /*----------------------------通常模式下的数据解包--------------------------------*/
	/*-----------------------参见协议0.1比赛时通讯包格式-----------------------------*/
	if((type == PACKET_Normal) && (g_robot.mode == NORMAL_MODE))
	{
		/* parse robot command */
		packet_robot_t packet;
    
		memset( &packet, 0, sizeof( packet ) ); //每个周期 下发速度值等清0

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
* Description	  : 解压手柄数据包 增加陀螺仪的控制输出用于小车平衡
*	              	
* Input		      : unsigned char *data
* Output		  : None
* Return		  : None.
*******************************************************************************/
int do_joystick_packet( unsigned char *data)
{   
		/* parse robot command */
		float x_k1,x_b1,x_k2,x_b2,r_k1,r_b1,r_k2,r_b2,y_k1,y_b1,y_k2,y_b2;
		unsigned char shoot_strength; //模拟量
		unsigned char button_key;
        unsigned char ctl_dibbler;//控球档位
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
		shoot_strength = data[17];//射门力度 LT按键 0~255
		if(shoot_strength < 3)
	    {
	      shoot_strength = 0;
	    }
		else
		{
		  shoot_strength = shoot_strength/2 + 3; //射门力度0~127 
		}
		
		ctl_dibbler = data[18];//控球档位模拟量  0~64 0档 64~128 1档 128~192 2档 192~256 3档
		
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
		   move_joystick_y = 1;//水平方向运动摇杆被操控
		}
		if((data[4]<=JOYSTICK_POS_LIMIT) && (data[4]>=JOYSTICK_NEG_LIMIT))
        {
          // data[4] = 0x80;
          joystick_packet.speed_x = 0;
		  move_joystick_x = 0;
          
        }
		else
		{
		   move_joystick_x = 1; //前后方向摇杆被操控
		}
		if((data[5]<=JOYSTICK_POS_LIMIT) && (data[5]>=JOYSTICK_NEG_LIMIT))
        {
          // data[4] = 0x80;
          joystick_packet.speed_rot = 0;  //控制小车旋转摇杆未操控而且控制小车前进的摇杆操控了则进行陀螺仪闭环控制
		  rot_joystick  = 1;  //手柄控制小车旋转摇杆未操控则满足条件1
          
        }
		else//旋转摇杆操控了则陀螺仪闭环控制取消
		{
		   rot_joystick = 0; 
		}
		/***北通左上角模拟量摇杆控制XY轴走***/
		if(data[4]>=JOYSTICK_POS_LIMIT)//手柄摇杆Y轴相当于小车X轴 前后方向 摇杆Y轴上0x00 下0xff 
		{
		   joystick_packet.speed_x = (int)((float)(x_k1*data[4])+x_b1);  //沿小车X轴向后运动
		}
		else if(data[4]<=JOYSTICK_NEG_LIMIT)//摇杆向上 小车沿着X轴向前运动
		{
		  joystick_packet.speed_x = (int)((float)(x_k2*data[4])+x_b2);
		}

		if(data[3]>=JOYSTICK_POS_LIMIT) //小车水平往右走
		{
		   joystick_packet.speed_y = (int)((float)(y_k1*data[3])+y_b1);
		}
		else if(data[3]<=JOYSTICK_NEG_LIMIT) //小车水平往左走
		{
		  joystick_packet.speed_y = (int)((float)(y_k2*data[3])+y_b2);
		}	
		//joystick_packet.speed_x = -(data[4]-0x80);
		//joystick_packet.speed_y = (data[3]-0x80);
        /***北通右下角模拟量摇杆控制旋转***/

		if(data[5]>=JOYSTICK_POS_LIMIT) //求取小车旋转速度
		{
		   joystick_packet.speed_rot = (int)((float)(r_k1*data[5])+r_b1);
		}
		else if(data[5]<=JOYSTICK_NEG_LIMIT)
		{
		  joystick_packet.speed_rot = (int)((float)(r_k2*data[5])+r_b2);
		}

	    for(;;)
	    {
			if(button_key &0x01) //X键按下 平射
			{
			  /* shoot */
			   shoot_delay_count++;
			   if(shoot_delay_count == 1)
			   {
				    shoot_current_time = get_sys_tick();
	    	        if(shoot_strength >= 127 ) //LT模拟量 射门力度
				      joystick_packet.shoot = MAX_SHOT_STRENGTH;
		            else
				      joystick_packet.shoot= shoot_strength;

					  break;
			   }
			   shoot_delay_time = get_sys_tick();
			   if((shoot_delay_time - shoot_current_time) > 2500)
			   {
			     	 shoot_delay_count = 0;
					 if(shoot_strength >= 127 ) //LT模拟量 射门力度
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
			else if((button_key >> 2) & 0x01)//B 按键按下 挑射
				{  
				   chip_delay_count++;
				   if(chip_delay_count == 1)
				   {
				     /* chip */
					  chip_current_time = get_sys_tick();
	    	         if(shoot_strength >=127 ) //LT模拟量 射门力度
				      joystick_packet.chip = MAX_SHOT_STRENGTH;
		             else
				      joystick_packet.chip= shoot_strength;
					 
					 break;
				   }
				   chip_delay_time = get_sys_tick();
			       if((chip_delay_time - chip_current_time) > 2500)
			       {
			         chip_delay_count = 0;
					 if(shoot_strength >=127 ) //LT模拟量 射门力度
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
			    else //平射挑射都没按下则射门力度清0
			    {
			      joystick_packet.shoot = 0;
				  joystick_packet.chip = 0;
				  break;
			    }
	    }

		
		/**控球档位**/
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
            gyro_control_flag = 1;  //控制小车前进后退的摇杆移动且控制小车旋转的摇杆未移动则进行陀螺仪闭环控制
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


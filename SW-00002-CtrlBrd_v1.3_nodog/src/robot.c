 /********************************************************************************
 ********************************************************************************
 * 权限:	杭州南江机器人股份有限公司
 * 文件名:	  robot.c 		
 * 功能描述:	
		  控制器主程序
 * 版本 	 作者		时间		  状态
 * V1.0 	 shouxian	2015.9.10	创建文件
 * V1.1      sunlifeng  2016.2.28   修改文件
 *****************************************************************************
 *****************************************************************************/
#include <string.h>
#include "math.h"
#include "arm_math.h"
#include "typedef.h"
#include "robot.h"
#include "cfg.h"
#include "action.h"
#include "timer.h"
#include "gpio.h"
#include "factory.h"
#include "param.h"
#include "pid.h"
#include "misc.h"
#include "motor.h"
#include "comm.h"
#include "usart_ble.h"


#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "usbh_hid_joystick.h"
#include "usbd_cdc_vcp.h"
#include "MPU6050_driver.h"
#include "error_event.h"
#include "packet.h"
/*************** Globals ****************/
robot_t g_robot;

int wheel_reduction_ratio_x_set; /*减速比*/		
int wheel_reduction_ratio_yz_set; /*减速比*/  //旧轮子减速比为70/22 为3.1818 外圈转1圈，内圈码盘转3.1818圈
int max_shot_strength_set;
unsigned char startpoint;
unsigned char steplength;

volatile char g_do_set_receive_mode_flag = 0;
volatile char g_set_receive_mode_flag = 0;

int forcestopcounter=0;
unsigned char last_beep = 0;

u8 is_low_power_cnt = 0;

u8 g_pid_timer_cnt = 0;
timer_t power_mon_timer;
timer_t heart_led_timer;
timer_t rf_comm_tim;           //发射机通信超时时间
timer_t identify_cpuid_tim;    //cpuid认证超时时间 设置为10S
timer_t gyro_mon_timer;

timer_t shoot_interval_timer;

extern char eeprom_blue_init_flag;
extern char bluetooth_name[BLUETOOTH_MAX_NAME];
extern char bluetooth_code[BLUETOOTH_MAX_CODE];

extern void UNLOCK_FLASH_RDP(void);
extern u8  cpuid_data[12];
extern unsigned char identify_success;

extern packet_robot_t joystick_packet;

extern char gyro_control_flag;

/*******************************************************************************
* Function Name   : init_robot
* Description	  : 初始化控制器相关的一些变量
*					
* Input		      : None
* Output		  : None
* Return		  : 返回值0.
*******************************************************************************/
int init_robot(void)
{
	param_t param;
	u8 mode;
	u8 freq;
	u8 num;
	u8 i;
	float angle;
	
  	float wheel_angle[ 4 ] = { 
		 D_WHEEL_ANGLE_FRONT,     //左前轮
		-D_WHEEL_ANGLE_FRONT,      //右前轮
		-D_WHEEL_ANGLE_BACK_2013,   //右后轮
		 D_WHEEL_ANGLE_BACK_2013     //左后轮轮
	};
		
	
	/* initial parameter from eeprom */
	load_param(&param);
	wheel_reduction_ratio_x_set = param.dat[12];
 	wheel_reduction_ratio_yz_set = param.dat[13];
    eeprom_blue_init_flag = param.dat[15];
	if(param.dat[14] <= MAX_SHOT_STRENGTH)
    {
    	max_shot_strength_set = param.dat[14];
    }
    else 
    {
    	max_shot_strength_set = MAX_SHOT_STRENGTH;
    }

	/* initial g_robot */
	read_dip_sw(&freq, &num, &mode);
	
	memset(&g_robot, 0, sizeof(g_robot));
	g_robot.num = num;
	g_robot.frq = freq;
	g_robot.mode = (mode_t)(mode & 0x7);
    mode = mode & 0x7;
	if(mode == 2) 
		g_robot.rf_mode = RF_BTE;
	else if(mode == 0)
		g_robot.rf_mode = RF_24L01;
	     else
	     {
	     }

	for(i = 0; i < CHANNEL_NUM; i++)
	{
		pid_init(&(g_robot.wheels[i].pid), MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
	}
	#if MPU6050_GYRO_USED
	gyro_pid_init(&gyro_pid, GYRO_PID_KP, GYRO_PID_KI, GYRO_PID_KD);
	#endif
	g_robot.dribbler = 0;

	g_robot.kv2n = ( (float)wheel_reduction_ratio_x_set + (float)wheel_reduction_ratio_yz_set 
		* 0.01f ) * ( 4 * ENCODER_COUNTS_PER_TURN_SET) / 2 / (float)PI / WHEEL_RADIUS;

	/* initial sin and cos table */
	for( i = 0; i < 2; i++ )
	{
	    angle = wheel_angle[ i ] / 180.0f * (float)PI;
	    g_robot.sin_angle[i] = sin( angle ) ;
	    g_robot.cos_angle[i] = cos( angle ) ;
	}
	
    for(i = 2; i < 4; i++)
    {
      	angle = wheel_angle[ i ] / 180.0f * (float)PI;
		    g_robot.sin_angle[i] = sin(angle) ;
      	g_robot.cos_angle[i] = cos(angle) ;
    }
    g_robot.firmware_version = software_verison;
	/* initial other */
	init_shooter();

	/* initial ir pwm */
	start_ir_pwm();

	return 0;
}

/*******************************************************************************
* Function Name   : int do_power_monitor(void)
* Description	  : 电池电压监控
*					
* Input		      : None
* Output		  : None
* Return		  : 返回1表示电池电压降到阈值1 返回2表示电池电压降到阈值2 返回0 电池电压正常
*******************************************************************************/
int do_power_monitor(void)
{
	char retflag;
	u8 cap_v;
	
	/* check the cap voltage */
	cap_v = get_cap_v();

	DIS_INT();
	g_robot.cap_v_f = cap_v * CAP_V_ADC_GAIN;
	g_robot.cap_v = cap_v;
	EN_INT();

	retflag = is_power_low();
    if( retflag )
	{ 
		if(is_low_power_cnt < 5)
			is_low_power_cnt++;
		else
		{
			is_low_power_cnt = 5;
		
#ifdef ENABLE_BEEP
			BEEP_ON();
#endif
			return retflag;
		}
	}
	else
	{
        is_low_power_cnt = 0;		
		
#ifdef ENABLE_BEEP
		if(last_beep == 0) BEEP_OFF();
#endif
	}

	return 0;
}


/*******************************************************************************
* Function Name   : do_timer
* Description	  : 中断周期调用,pid计算
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void do_timer()
{
	/* do motor pid control */
	g_pid_timer_cnt++;
	if(g_pid_timer_cnt >= PID_COUNTER_OVERFLOW)
	{
		update_motor();
		g_pid_timer_cnt = 0;
	}
	
	if(g_do_set_receive_mode_flag)
	{
		g_set_receive_mode_flag++;
	}
}


/*******************************************************************************
* Function Name   : do_run
* Description	  : 进入主循环，根据选择拨码盘的不同，选择不同的工作模式。
*支持比赛模式(发射机通信PC),手柄模式(游戏手柄操控),蓝牙模式(手机操控),USB通信模式(debug模式PC机的参数下发),自检模式					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void do_run(void)
{
	int start_beep_flag = 1;
	static int mode_count = 0;
	int i;
	
	/* at begining, check if do beep when start */
	if(start_beep_flag)
	{
		i = 0;
		if(g_robot.mode <= DEBUG_MODE)	
		{
			i = g_robot.mode + 1;		
		}
		
		while(i != 0)
		{
			BEEP_ON();
			wait_ms(200);
			BEEP_OFF();
			wait_ms(200);
			i--;
		}
		start_beep_flag = 0;
	}

	/* initial timer */
	power_mon_timer = get_one_timer(POWER_MON_TIME);
	heart_led_timer = get_one_timer(HEARTBEAT_TIME);
	rf_comm_tim = get_one_timer(COMM_TIMEOUT_TIME); //无线通信模式超时时间
	shoot_interval_timer = get_one_timer(1);
	identify_cpuid_tim =
  get_one_timer(IDENTIFY_CPUID_TIMEOUT_TIME); //cpuid认证超时时间设置为10s 10s认证不成功则停止机器人
	gyro_mon_timer = get_one_timer(GYRO_MON_TIME);
	/* start motor */
	start_motor();
	
	while(1)
	{
   
	   	
		/* check battery voltage */
#ifdef ENABLE_POWERMON
        
		if(check_timer(power_mon_timer))
		{
			power_mon_timer = get_one_timer(POWER_MON_TIME);
			if(do_power_monitor() == 2)//电池电压降低到设定阈值1 则蜂鸣器报警 降到阈值2则停止机器人
			{
				/* battery voltage is too low, I will stop the robot */
				forcestopcounter++;
				if(forcestopcounter >= LOW_POWER_TIME)
				{
					do_dribbler(0);
					do_move(0,0,0);
					do_shoot(0,0);
					do_chip(0,0);
					while(1) ; /* stop robot */
				}
			}
			else
			{
				if(forcestopcounter > 0) forcestopcounter--;
			}
		}
#endif

#ifdef ENABLE_HEARTBEAT
		/* heart led */
		if(check_timer(heart_led_timer))
		{
			//heart_beat();
			heart_led_timer = get_one_timer(HEARTBEAT_TIME);
		}
#endif
    
	
		/* do robot job */
		switch(g_robot.mode)
		{ 
		    case BlUETOOTH_MODE:
				mode_count++;
				LED_POWER_ON_HIGH();
				set_heart_led(0, 0, 100, 0x1f);//蓝色
				
			case NORMAL_MODE :
			{
				if(mode_count == 1)//
				{
				   mode_count = 0;
				}
				else
				{  
				    //LED_POWER_ON_HIGH();
				    //set_heart_led(100, 0, 0, 0x1f);//红色
				    LED_POWER_ON_LOW();//熄灭色彩灯
				}
				/* process message from comm device( rs232, wireless, etc. ) */
				do_comm();

			    /*通讯超时设置为500ms或者什么都不做 上位机8ms下发一个数据包 (共2包一包(25Byte)给一队车，另外一包给另一队车)*/
				if(check_timer(rf_comm_tim)) 
				{
					/* 防止通讯中断，置位设置通讯为接收模块标志位 */
					g_do_set_receive_mode_flag = 1; 
					
					do_dribbler(0);
					do_move(0,0,0);
					do_shoot(0,0);
					do_chip(0,0);

					start_nRF24L01_RX();	
					rf_comm_tim = get_one_timer(COMM_TIMEOUT_TIME);
					identify_cpuid_tim = get_one_timer(IDENTIFY_CPUID_TIMEOUT_TIME);
				}
//				if(check_timer(identify_cpuid_tim))//身份识别超时或者身份识别不通则停止机器人
//				{
//				    /* 防止通讯中断，置位设置通讯为接收模块标志位 */
//					g_do_set_receive_mode_flag = 1; 
//					
//					do_dribbler(0);
//					do_move(0,0,0);
//					do_shoot(0,0);
//					do_chip(0,0);

//					start_nRF24L01_RX();
//					identify_success = 0;
//					identify_cpuid_tim = get_one_timer(IDENTIFY_CPUID_TIMEOUT_TIME);
//				}
				
				if(g_do_set_receive_mode_flag)	//发送数据包后置1等待数据发送出去后将模式修改为接收模式				
				{
					/* 将通讯设置为接收模式，并置位可接受标志位 */
					if(g_set_receive_mode_flag >= 3)
					{				
						start_nRF24L01_RX();
					
						g_set_receive_mode_flag = 0;
						g_do_set_receive_mode_flag = 0; 	//置位可接收标志位
					}
				}

				break;
			}
			case JOYSTICK_MODE:
			{
			/*北通游戏手柄控制*/
#ifdef ENABLE_JOYSTICK_CONTROL
				
               
				LED_POWER_ON_HIGH();
				set_heart_led(0, 100, 0, 0x1f);//绿色
				/* Host Task handler */
				USBH_Process(&USB_OTG_Core_dev , &USB_Host);
				if(HID_JOYSTICK_Data.packet_valid == 1)
				{
					HID_JOYSTICK_Data.packet_valid = 0;
					do_joystick_packet((unsigned char*)(&HID_JOYSTICK_Data.key_button));
					
				}
				
				if(check_timer(gyro_mon_timer))
				{  
				   
				   if(gyro_control_flag ==1)
				   {
				   	 Get_Motion_data(&acc_gyro_adc,&acc_gyro_actul);
				     gyro_pid.feedback_speed = -acc_gyro_actul.GYRO_z;  //陀螺仪Z轴旋转顺时针为负，与小车顺时针旋转值为正相反 取负号
				   
				     gyro_control_calcuate(&gyro_pid);
				   }   
				  
		
				   gyro_mon_timer = get_one_timer(GYRO_MON_TIME);
				}
             
				break;
#endif
			}
			case SELFTEST_MODE: //自检模式
			{    
				static int test_time = 0;
				LED_POWER_ON_HIGH();
				set_heart_led(100, 100, 100, 0x1f);//白色			
				test_time++;
				COMM_LED_ON();
				wait_ms(2000);
			    COMM_LED_OFF();	
				set_test_shooter();
                do_dribbler(1);//设置控制档位
                wait_ms(2000);
				do_dribbler(0);
				do_shoot(20, 0);//平射
		        wait_ms(2000);
				do_chip(0, 20); //挑射
                wait_ms(2000);
				
                if(test_time == 1)
                {
		         do_acc_handle_move(0, 0,100);
				 wait_ms(2000);
				 do_acc_handle_move(0, 0,0);
				 
                }
				else if(test_time == 2)
					{
					 do_acc_handle_move(0, 0,-100);
				     wait_ms(2000);
				     do_acc_handle_move(0, 0,0);
					 test_time = 0;
					}
                   

				
				break;
				

			}
			case DEBUG_MODE: //PC机下发参数模式
			{
                LED_POWER_ON_HIGH();
                set_heart_led(100, 100, 0, 0x1f);//黄色
                if(USB_Receive_ok == 1) //usb 接收到数据
                {
                   int i = 0;
				   int rtn = 0;
				  switch(USB_Receive_Buffer[0])
				  {
                   	 case SET_BLUETOOTH_NAME:   //上传1Byte
					 {
						for(i = 0; i < BLUETOOTH_MAX_NAME; i++)
						{
						  bluetooth_name[i]=USB_Receive_Buffer[i+1];
						}
                        
						 rtn = AT_Set_device_name_cmd(bluetooth_name);
						 if(rtn == 1) //设置OK
						 {
						   APP_Rx_Buffer[APP_Rx_ptr_in] = 1; //返回1 主机收到1表示设置成功
						 }
						 else
						 {
						   APP_Rx_Buffer[APP_Rx_ptr_in] = 0; //返回0表示 主机设置参数失败
						 }
						  APP_Rx_ptr_in++;
						 
	                      // To avoid buffer overflow 
                         if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                         {
                           APP_Rx_ptr_in = 0;
                         }  
	                        		
						break;
                   	 }
					 case SET_BLUETOOTH_CODE: //上传1Byte
					 {
						
                         for(i = 0; i < BLUETOOTH_MAX_CODE; i++)
						 {
						   bluetooth_code[i]=USB_Receive_Buffer[i+1];
						 }
                        
						 rtn = AT_Set_device_name_code(bluetooth_code);
						 if(rtn == 1) //设置OK
						 {
						   APP_Rx_Buffer[APP_Rx_ptr_in] = 1; //返回1 主机收到1表示设置成功
						 }
						 else
						 {
						   APP_Rx_Buffer[APP_Rx_ptr_in] = 0; //返回0表示 主机设置参数失败
						 }
						  APP_Rx_ptr_in++;
						 
	                      // To avoid buffer overflow 
                         if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                         {
                           APP_Rx_ptr_in = 0;
                         }  
						

						break;
					 }
					 case SET_MOTOR_PID:
					 {
					 	break;
					 }
					 case GET_CONTROL_STATUS:  //获取控制器状态 上传3Byte
					 {
                     
                         APP_Rx_Buffer[APP_Rx_ptr_in] = (g_robot.firmware_version >> 8)& 0xff; //
                         APP_Rx_ptr_in++;
                         APP_Rx_Buffer[APP_Rx_ptr_in] = g_robot.firmware_version & 0xff; //
                         APP_Rx_ptr_in++;
						 APP_Rx_Buffer[APP_Rx_ptr_in]  = error_flag.all;
                         APP_Rx_ptr_in++;
                         if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                         {
                           APP_Rx_ptr_in = 0;
                         }        
						break;
					 }
					 case GET_CPU_ID:  //获取控制器CPU唯一ID 上传12Byte
					 {  
                       
					     for(i = 0 ; i < 12; i ++)//低字节先发送
					     {  
					         APP_Rx_Buffer[APP_Rx_ptr_in] = cpuid_data[i];
							 APP_Rx_ptr_in++;   
					     }
						
                         if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                         {
                           APP_Rx_ptr_in = 0;
                         }        
						break;
					 }
					 case SET_UNLOCK_FALSH_RDPROTECTION:  //解除flash读保护机制 上传1Byte
					 {

						  if(USB_Receive_Buffer[1] == 'F' ) //通过usb下发数据 0x05 0x46 则对flash读保护解除 flash程序擦除 谨慎操作
                          {
						      UNLOCK_FLASH_RDP();
						      APP_Rx_Buffer[APP_Rx_ptr_in] = 1; //返回1 主机收到1表示设置成功
						      APP_Rx_ptr_in++;
                          }
						  if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                          {
                              APP_Rx_ptr_in = 0;
                          }        
					 }
					 case GET_CAP_VOLATAGE:   //上传4Byte
					 {
					 	 short cap_v_int = 0; //实际电压扩大100倍
						 short bat_v_int = 0;
						 cap_v_int = (short)(g_robot.cap_v_f * 100);
						 bat_v_int = (short)(g_robot.bat_v_f * 100);
					 	 APP_Rx_Buffer[APP_Rx_ptr_in] = (u8)(cap_v_int & 0xff);
						 APP_Rx_ptr_in++;
						 APP_Rx_Buffer[APP_Rx_ptr_in] = (u8)((cap_v_int >> 8) & 0xff);
						 APP_Rx_ptr_in++;
						 APP_Rx_Buffer[APP_Rx_ptr_in] = (u8)(bat_v_int & 0xff);
						 APP_Rx_ptr_in++;
						 APP_Rx_Buffer[APP_Rx_ptr_in] = (u8)((bat_v_int >> 8)& 0xff);
						 APP_Rx_ptr_in++;
						  if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                          {
                           APP_Rx_ptr_in = 0;
                          }        
						break;
					 }
					 case SET_ROBOT_DO_SHOOT:
					 {
					 	  u8 strength = 0;
                          if(USB_Receive_Buffer[1] == 'S' ) // shoot通过usb下发数据 0x08 0x53 则执行平射动作 第3个byte平射力量
                          {   
                              strength = USB_Receive_Buffer[2];
						      do_shoot(strength, 0);//平射
						      APP_Rx_Buffer[APP_Rx_ptr_in] = 1; //返回1 主机收到1表示设置成功
						      APP_Rx_ptr_in++;
                          }
						  else if(USB_Receive_Buffer[1] == 'C')//chip 0x08 0x43 执行挑射
						  	   {
						  	     strength =  USB_Receive_Buffer[2];
								 do_chip(0, strength);//挑射
						  	     APP_Rx_Buffer[APP_Rx_ptr_in] = 1; //返回1 主机收到1表示设置成功
						         APP_Rx_ptr_in++;
						  	   }
						  if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
                          {
                           APP_Rx_ptr_in = 0;
                          }   
						 break;
					 }

					  default:
					  	break;
                   }



				   USB_Receive_ok = 0;
                }
				
				break;

			}
				
			default: break;
		}

		/* 对射门完成之后的延时进行计时 */
#ifdef ENABLE_SHOOTER
        if(g_robot.mode != SELFTEST_MODE)
        {
           if(check_timer(shoot_interval_timer)) 
		   {
			 update_shooter();
			 shoot_interval_timer = get_one_timer(1);
		   }
        }
		
#endif




	}	
}


/*******************************************************************************
* Function Name   : SysTick_Handler
* Description	  : 系统时钟中断函数
*					
* Input		      : None
* Output		  : None
* Return		  : None
*******************************************************************************/
void SysTick_Handler(void)
{
	/* not need disable int, because the same int of ARM cortex-M3 cannot
	   be nested */
    static int watch_dog_tick = 0;
	watch_dog_tick++;
	if(watch_dog_tick >=1500)
	{
	  feed_iwdog();
	  watch_dog_tick = 0;
	}
	update_sys_timer();
	do_timer();

}




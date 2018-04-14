/********************************************************************************
********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:    Hm_bluetooth.c         
* ��������:    
       ����ģ�鴮��ͨ��Э��
* �汾      ����       ʱ��          ״̬
* V1.0      sunlf      2015.10.14    �����ļ�
*****************************************************************************
*****************************************************************************/
#include "Hm_bluetooth.h"
#include "usart_ble.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"
#include "Timer.h"

#include <string.h>


extern u32 cpuID[3];
extern u8 buf_packet_ok[];
extern u8 ble_ack_ok;
extern void SendString_USART3(u8* str);

extern int get_uart_ble_ack_packet(u8 *buf);

u8 Hm_bluetooth_connect_flag = 0;
char bluetooth_name[BLUETOOTH_MAX_NAME];
char bluetooth_code[BLUETOOTH_MAX_CODE];

/*******************************************************************************
* Function Name   : AT_test_mode
* Description	  : HM-13����ģ����Ժ���
*					
* Input		      : None
* Output		  : None
* Return		  : ����1�ɹ�����0ʧ��.
*******************************************************************************/
int AT_test_mode(void)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
   int rtn = 1;
   
   SendString_USART3(TEST_BLE);            //test mode   
   ftime_out = get_sys_tick() + 100;		//time out is 100ms
    while(1)   //�������ģ���Ƿ�OK 
    {
    		
      wait_ms(20);  //ȷ���Ѿ��յ�ACK����
      if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
      {
        if(ble_ack_ok == 1)
        {
           ble_ack_ok = 0;
	          rtn &= 1;
			  break;
        }
	    else
	    {
	        cur_time = get_sys_tick();
	        if(cur_time > ftime_out)
	        {
	        	rtn &= 0;
				break;
	        }
	 	      
	    }
      }
      else
      {
         cur_time = get_sys_tick();
	       if(cur_time > ftime_out)
	 	   {
	          rtn &= 0;
			  break;
	       }
      }

    }
	return rtn;
}

/*******************************************************************************
* Function Name   : AT_Set_device_name
* Description	  : ��������ģ��EDR BLE ����
*					
* Input		      : None
* Output		  : None
* Return		  : ����1�ɹ�����0ʧ��.
*******************************************************************************/
int AT_Set_device_name(void)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
   int cpuid_24 = 0;
   char s_buf[10] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
   int i = 0;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         cpuid_24 = cpuID[2]&0xffffff;
		 s_buf[0] =  'N';
		 s_buf[1] =  'J';
		 s_buf[2] =  '_';
		 s_buf[3] = (cpuid_24 >> 20 );
		 s_buf[4] = ((cpuid_24 >> 16)&0x0f) ;
		 s_buf[5] = ((cpuid_24 >> 12)&0x0f );
		 s_buf[6] = ((cpuid_24 >> 8)&0x0f) ;
		 s_buf[7] = ((cpuid_24 >> 4)&0x0f) ;
		 s_buf[8] = (cpuid_24 &0x0f );

		 for(i = 3; i < 9; i++)
	     {
	       if(s_buf[i]<10)//0~9
	       {
		 	 s_buf[i] = s_buf[i] + '0';
	       }
		   else //A~F
		   {
		     s_buf[i] = s_buf[i] + 55;
		   }
	     }
		 strcat(&hm_bule_send_buf[0],SET_EDR_DEVICE_NAME);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				    rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		           	   rtn &= 0;  //��ʱʧ��
		           	   break;
		           }
		 	          
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	  rtn &= 0;
		         	  break;
		         }
		 	        
	       }
         }
				 
		 //����  BLE�豸����
		 memset((void *)&hm_bule_send_buf[0], 0, sizeof(hm_bule_send_buf));
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         cpuid_24 = cpuID[2]&0xffffff;
		 s_buf[0] =  'N';
		 s_buf[1] =  'J';
		 s_buf[2] =  '_';
		 s_buf[3] = (cpuid_24 >> 20 );
		 s_buf[4] = ((cpuid_24 >> 16)&0x0f) ;
		 s_buf[5] = ((cpuid_24 >> 12)&0x0f );
		 s_buf[6] = ((cpuid_24 >> 8)&0x0f) ;
		 s_buf[7] = ((cpuid_24 >> 4)&0x0f) ;
		 s_buf[8] = (cpuid_24 &0x0f );

		 for(i = 3; i < 9; i++)
	     {
	       if(s_buf[i]<10)//0~9
	       {
		 	 s_buf[i] = s_buf[i] + '0';
	       }
		   else //A~F
		   {
		     s_buf[i] = s_buf[i] + 55;
		   }
	     }
		 strcat(&hm_bule_send_buf[0],SET_BLE_DEVICE_NAME);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		           break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		           	   rtn &= 0;
					   break;
		           }
		 	       
		       }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	rtn &= 0;
					 break;
		         }
		 	       
	       }
         }
		 return rtn;
}

/*******************************************************************************
* Function Name   : AT_Set_device_name_cmd
* Description	  : ��������ģ��EDR BLE ����
*					
* Input		      : None
* Output		  : None
* Return		  : ����1�ɹ� ����0ʧ��.
*******************************************************************************/
int AT_Set_device_name_cmd(char *name)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
   int cpuid_24 = 0;
   char s_buf[10] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         memcpy(&s_buf[0], name, sizeof(s_buf));
		 		 
		 strcat(&hm_bule_send_buf[0],SET_EDR_DEVICE_NAME);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				    rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		           	   rtn &= 0;
		           	   break;
		           }
		 	          
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	  rtn &= 0;
		         	  break;
		         }
		 	        
	       }
         }
				 
		 //����  BLE�豸����
		 memset((void *)&hm_bule_send_buf[0], 0, sizeof(hm_bule_send_buf));
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         
         memcpy(&s_buf[0], name, sizeof(s_buf));
		
		 strcat(&hm_bule_send_buf[0],SET_BLE_DEVICE_NAME);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		           break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		           	   rtn &= 0;
					   break;
		           }
		 	       
		       }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	rtn &= 0;
					 break;
		         }
		 	       
	       }
         }
		 return rtn;
}
/*******************************************************************************
* Function Name   : AT_Set_device_name
* Description	  : ��������ģ��EDR BLE ƥ������
*					
* Input		      : None
* Output		  : None
* Return		  : ����1�ɹ�����0ʧ��.
*******************************************************************************/
int AT_Set_device_name_code(char *code)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
   int cpuid_24 = 0;
   char s_buf[6] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
    //���� EDR�豸ƥ������
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         memcpy(&s_buf[0], code, sizeof(s_buf));
		 		 
		 strcat(&hm_bule_send_buf[0],SET_EDR_DEVICE_CODE);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				    rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		           	   rtn &= 0;
		           	   break;
		           }
		 	          
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	  rtn &= 0;
		         	  break;
		         }
		 	        
	       }
         }
				 
		 //����  BLE�豸ƥ������
		 memset((void *)&hm_bule_send_buf[0], 0, sizeof(hm_bule_send_buf));
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         
         memcpy(&s_buf[0], code, sizeof(s_buf));
		
		 strcat(&hm_bule_send_buf[0],SET_BLE_DEVICE_CODE);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		           break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		           	   rtn &= 0;
					   break;
		           }
		 	       
		       }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	rtn &= 0;
					 break;
		         }
		 	       
	       }
         }
		 return rtn;
}

/*******************************************************************************
* Function Name   : void AT_Set_baudrate(int baudrate)
* Description	  : ��������ģ�鴮�ڲ�����
*					
* Input		      : int baudrate
* Output		  : None
* Return		  : ����1�ɹ�����0ʧ��.
*******************************************************************************/
int AT_Set_baudrate(int baudrate)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
   char s_buf[2] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         	
		 s_buf[0] = baudrate + '0';
		
		 
		 strcat(&hm_bule_send_buf[0],SET_MODULE_BAUDTATE);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &= 0;
		 	          break;
		           }
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		            rtn &= 0;
		 	        break;
		         }
	       }
         }
				 
		 return rtn;
	     
}
/*******************************************************************************
* Function Name   : void AT_Set_baudrate(int baudrate)
* Description	  : ����pio1���״̬ AT+PIO1[] 1 �����Ʋ��� ���Ӻ��� 0 �������������Ӻ���
*					
* Input		      : int baudrate
* Output		  : None
* Return		  : ����1�ɹ�����0ʧ��.
*******************************************************************************/
int AT_Set_Pio1_status(int status)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
   char s_buf[2] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
   
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         	
		 s_buf[0] = status + '0';
		
		 
		 strcat(&hm_bule_send_buf[0],SET_PIO1_LED_STATUS);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &=0;
		 	          break;
		           }
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		            rtn &=0;
		 	        break;
		         }
	       }
         }

		 return rtn;
				 
		
	     
}

/*******************************************************************************
* Function Name   : void AT_Set_connect_mode(int mode)
* Description	  : ��������ģ������ģʽ
*					
* Input		      : int mode
* Output		  : None
* Return		  : None.
*******************************************************************************/
int AT_Set_connect_mode(int mode)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
	 char s_buf[2] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn =1;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         	
		 s_buf[0] = mode + '0';
		
		 
		 strcat(&hm_bule_send_buf[0],SET_MODULE_CONNECT_MODE);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &=0;
		 	          break;
		           }
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		            rtn &=0;
		 	        break;
		         }
	       }
         }
				 
		return rtn;
	     
}

/*******************************************************************************
* Function Name   : void AT_Set_data_speed(int speed)
* Description	  : ��������ģ�����ݴ����ٶ�
*					
* Input		      : int speed
* Output		  : None
* Return		  : None.
*******************************************************************************/
int AT_Set_data_speed(int speed)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
	 char s_buf[2] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         	
		 s_buf[0] = speed + '0';
		
		 
		 strcat(&hm_bule_send_buf[0],SET_MODULE_DATA_SPEED);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &=0;
		 	          break;
		           }
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		            rtn &=0;
		 	        break;
		         }
	       }
         }
		 return rtn;
				 
		
	     
}

/*******************************************************************************
* Function Name   : AT_Set_module_work_mode(int work_mode)
* Description	  : ��������ģ���ϵ繤��ģʽ
*					
* Input		      : int speed
* Output		  : None
* Return		  : None.
*******************************************************************************/
int AT_Set_module_work_mode(int work_mode)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
	 char s_buf[2] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn = 1;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         	
		 s_buf[0] = work_mode + '0';
		
		 
		 strcat(&hm_bule_send_buf[0],SET_EDR_WORK_MODE);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				 rtn &= 1;
		            break;
	          }
		       else
		       {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &= 0;
		 	          break;
		           }
		       }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		         	rtn &= 0;
		 	        break;
		         }
	       }
         }


		 memset((void *)&hm_bule_send_buf[0], 0, sizeof(hm_bule_send_buf));
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         
		 s_buf[0] = work_mode + '0';
		 
		 
		 strcat(&hm_bule_send_buf[0],SET_BLE_WORK_MODE);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				  rtn &= 1;
		            break;
	          }
		      else
		      {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &= 0;
		 	          break;
		           }
		      }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
		            rtn &= 0;
		 	        break;
		         }
	       }
         }

		 return rtn;
		
	     
}
/*******************************************************************************
* Function Name   : AT_Set_notify_mode(int notify_mode)
* Description	  : ��������ģ�����ӳɹ���֪ͨ����
*					
* Input		      : int notify_mode
* Output		  : None
* Return		  : None.
*******************************************************************************/
int AT_Set_notify_mode(int notify_mode)
{
   vu64 cur_time =0;
   vu64 ftime_out = 0;
	 char s_buf[2] = {0};
   char hm_bule_send_buf[20] = {0};
   int rtn =1;
    //���� EDR�豸����
         ftime_out = get_sys_tick() + 100;		//time out is 100ms
         	
		 s_buf[0] = notify_mode + '0';
		
		 
		 strcat(&hm_bule_send_buf[0],SET_NOTIFY_CONNECT_STATUS);
		 strcat(&hm_bule_send_buf[0],&s_buf[0]);
         SendString_USART3((u8 *)&hm_bule_send_buf[0]);
		
         while(1)
         {
           wait_ms(20);
	       if(get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)
	       {
	          if(ble_ack_ok == 1)
	          {
	             ble_ack_ok = 0;
				  rtn &= 1;
		            break;
	          }
		       else
		       {
		           cur_time = get_sys_tick();
		           if(cur_time > ftime_out)
		           {
		              rtn &= 0;
		 	          break;
		           }
		        }
	       }
	       else
	       {
	           cur_time = get_sys_tick();
		         if(cur_time > ftime_out)
		         {
                    rtn &=0;
					break;
		         }
	       }
         }
		 return rtn ;
				 
		
	     
}

/*******************************************************************************
* Function Name   : Init_hm_blue_module
* Description	  : ��ʼ��HM-13 ����ģ��
*					
* Input		      : None
* Output		  : None
* Return		  : ����1���� 0ʧ��.
*******************************************************************************/
int Init_hm_blue_module(void)
{  

	   int rtn = 1; 
#ifdef ENABLE_BLE_FUN

         AT_test_mode();
     
        rtn &= AT_Set_device_name();

		rtn &= AT_Set_Pio1_status(IDLE_LED_OFF);

		rtn &= AT_Set_baudrate(BAUDRATE_115200);

		rtn &= AT_Set_connect_mode(CONNECT_TWO_TXRX);

		rtn &= AT_Set_data_speed(DATA_SPEED_LOW);

		rtn &= AT_Set_module_work_mode(EDR_WORK_AUTO);

		rtn &= AT_Set_notify_mode(CONNECT_NOTIFY_YES);
        
		

		return rtn;		 
   #endif     
}

/*******************************************************************************
* Function Name   : hm_bluetooth_connect_status
* Description	  : ��ȡ��������״̬
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
int hm_bluetooth_connect_status(void)
{  

	  int ret = 0;
#ifdef ENABLE_BLE_FUN

       
	  
	      if((get_uart_ble_ack_packet(&buf_packet_ok[0]) == 0)) //��ȡ OK+CONE  �ַ���
	      {
	      	
			  if(ble_ack_ok == 1)
			  {
				 ble_ack_ok = 0;
				 if(((buf_packet_ok[3]=='C')&&(buf_packet_ok[4]=='O')&&(buf_packet_ok[5]=='N')&&(buf_packet_ok[6]=='E'))||
				 	((buf_packet_ok[3]=='C')&&(buf_packet_ok[4]=='O')&&(buf_packet_ok[5]=='N')&&(buf_packet_ok[6]=='B')) )
				 {
				    ret = 1;  //�������ӳɹ�
				    Hm_bluetooth_connect_flag = 1;
					return ret;
				 }
				 else if(((buf_packet_ok[3]=='L')&&(buf_packet_ok[4]=='S')&&(buf_packet_ok[5]=='T')&&(buf_packet_ok[6]=='E'))||
					     ((buf_packet_ok[3]=='L')&&(buf_packet_ok[4]=='S')&&(buf_packet_ok[5]=='T')&&(buf_packet_ok[6]=='B')))
				 {
					ret = 0; //�����Ѿ��Ͽ�
					Hm_bluetooth_connect_flag = 0;
					return ret;
				 }
								
			  }
	      }
			
	      if(Hm_bluetooth_connect_flag ==1)
		  {
			 return 1;
		  }
		  else if(Hm_bluetooth_connect_flag ==0)
		  {
			 return 0;
		  }
				 
	
	   return ret;
		 
   #endif     
}



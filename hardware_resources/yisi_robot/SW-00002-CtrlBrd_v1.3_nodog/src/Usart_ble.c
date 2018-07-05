/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    Usart_ble.c          
* 功能描述:    
          蓝牙模块串口通信协议
* 版本      作者       时间          状态
* V1.0      sunlf      2015.04.10    创建文件
*****************************************************************************
*****************************************************************************/ 

#include "usart_ble.h"
#include "String.h" 

u8 uart_rx_buf[UART_RX_BUF_SIZE];   //串口接收缓存
u8 uart_tx_buf[UART_TX_BUF_SIZE];   //串口发送缓存
u8 buf_packet_ok[UART_PCKT_RX_SIZE];
u8 uart_rx_buf_cnt = 0;
u8 uart_tx_buf_cnt = 0;
u8 uart_rx_buf_ptr = 0;
u8 ble_ack_ok = 0;
volatile char g_is_uart_get_a_packet = 0;  //串口接收到PC机数据包标志位
volatile char g_is_uart_sent_a_packet = 0;  //串口发送到PC机数据包标志位
volatile char g_is_uart_get_packet_num = 0;
volatile char g_is_uart_sent_packet_num = 0;



/*******************************************************************************
* Function Name   : Initcom3
* Description	  : 串口3通信的初始化,用于蓝牙模块通信
*					
* Input		      : USART_BAUDRATE_EMUN baudrate
* Output		  : None
* Return		  : None.
*******************************************************************************/

void Init_com3(USART_BAUDRATE_EMUN baudrate)
{  
  
   USART_InitTypeDef USART_InitStructure;
  

   //BaudRate--默认  待定参数
   USART_InitStructure.USART_BaudRate = baudrate;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
   /* USART configuration */
   USART_Init(USART3, &USART_InitStructure);
   //USART_ClearFlag(USART3,USART_FLAG_RXNE);
   //USART_ClearITPendingBit(USART3, USART_IT_RXNE);
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//使能串口接收中断   /* Enable USART */
   //USART_ITConfig(USART3, USART_IT_TXE, ENABLE);//使能发送缓冲空中断
   USART_Cmd(USART3, ENABLE);

       
}

/*******************************************************************************
* Function Name   : SendString_USART3
* Description	  : UART3 发送字符串
*					
* Input		      : u8* str
* Output		  : None
* Return		  : None.
*******************************************************************************/

void SendString_USART3(u8* str)
{
  
  while((*str)!=0)
  {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); //TC硬件置1发送完成 
    USART_SendData(USART3, *str);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
    str++;
  }
 
}

/*******************************************************************************
* Function Name   : SendChar_USART3
* Description	  : uart3 发送指定长度字符串
*					
* Input		      : u8* buffer,u8 len
* Output		  : None
* Return		  : None.
*******************************************************************************/
void SendChar_USART3(u8* buffer,u8 len)
{
   
  while(len--)
  {
    
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  
     USART_SendData(USART3, *buffer);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
    buffer++;
	
  }
  
}

/*******************************************************************************
* Function Name   : get_uart_ble_ack_packet
* Description	  : uart3 获取蓝牙模块被设置AT命令时候的返回包
*					
* Input		      : u8* buffer,u8 len
* Output		  : None
* Return		  : 返回0成功 返回-1失败.
*******************************************************************************/

int get_uart_ble_ack_packet(u8 *buf)
{
	u8 buf_rd[UART_BLE_ACK_MAX_PCKT_SIZE] = {0};
	u8 i;
	int buf_ptr;
	u16 n;
		
	while(1)
	{
		/* if there is some data in buffer */
		DIS_INT();
		if((uart_rx_buf_cnt >= UART_HM_BLE_ACK_L)&&(uart_rx_buf_cnt < UART_BLE_ACK_MAX_PCKT_SIZE))
		{
			buf_ptr = uart_rx_buf_ptr - uart_rx_buf_cnt;
			if(buf_ptr < 0) buf_ptr += UART_RX_BUF_SIZE;

			/* read the packet */
			n = 0;
			for(i = 0; i < uart_rx_buf_cnt; i++)
			{
				buf_rd[n++] = uart_rx_buf[buf_ptr];
				buf_ptr = (buf_ptr + 1) % UART_RX_BUF_SIZE;
			}

			/* check the packet */
			if((buf_rd[0] == 'O')&&(buf_rd[1] == 'K'))//应答帧头
			{
		
				memcpy(buf, buf_rd, UART_BLE_ACK_MAX_PCKT_SIZE);
				uart_rx_buf_cnt = 0;
			    ble_ack_ok = 1;
			
				EN_INT();
				return 0;
				
			}
			if(buf_rd[0] == UART_PACK_FRAME_HEAD) //正常数据帧 不在应答帧处理
			{
				EN_INT();
		       return -1;
			}
		
			/* this is a bad packet, uart data cnt-- */
			uart_rx_buf_cnt--;
			EN_INT();
		}
		else
		{
			EN_INT();
			return -1;
		}
	}

	
}

/*******************************************************************************
* Function Name   : get_uart_ble_packet
* Description	  : uart3 获取蓝牙模块接收到的数据
*					
* Input		      : u8* buffer,u8 len
* Output		  : None
* Return		  : None.
*******************************************************************************/

int get_uart_ble_packet(u8 *buf)
{
	u8 buf_rd[UART_PCKT_RX_SIZE];
	u8 i;
	int buf_ptr;
	u16 n;
		
	while(1)
	{
		/* if there is some data in buffer */
		DIS_INT();
		if(uart_rx_buf_cnt >= UART_PCKT_RX_SIZE)
		{
			buf_ptr = uart_rx_buf_ptr - uart_rx_buf_cnt;
			if(buf_ptr < 0) buf_ptr += UART_RX_BUF_SIZE;

			/* read the packet */
			n = 0;
			for(i = 0; i < uart_rx_buf_cnt; i++)
			{
				buf_rd[n++] = uart_rx_buf[buf_ptr];
				buf_ptr = (buf_ptr + 1) % UART_RX_BUF_SIZE;
			}

			/* check the packet */
			if(buf_rd[0] == UART_PACK_FRAME_HEAD)//帧头
			{
		
				memcpy(buf, buf_rd, UART_PCKT_RX_SIZE);
				uart_rx_buf_cnt = uart_rx_buf_cnt - UART_PCKT_RX_SIZE;
				g_is_uart_get_a_packet = 1;
				g_is_uart_get_packet_num ++;
				EN_INT();
				return 0;
				
			}

			/* this is a bad packet, uart data cnt-- */
			uart_rx_buf_cnt--;
			EN_INT();
		}
		else
		{
			EN_INT();
			return -1;
		}
	}

	
}

/*******************************************************************************
* Function Name   : USART3_IRQHandler
* Description	  : usart3 中断函数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void USART3_IRQHandler(void)
{
	//DIS_INT();

	if(USART_GetITStatus(UART_NUM, USART_IT_RXNE) != RESET)
	{
		if(uart_rx_buf_cnt >= UART_RX_BUF_SIZE)
		{   
		    uart_rx_buf_cnt = 0;
			goto exit;
		}

		/* Read one byte from the receive data register */
		uart_rx_buf[uart_rx_buf_ptr] = USART_ReceiveData(UART_NUM);
		uart_rx_buf_ptr = (uart_rx_buf_ptr + 1) % UART_RX_BUF_SIZE;
		uart_rx_buf_cnt++;
	}
	
exit:
	/* clear IT flag */
	USART_ClearFlag(UART_NUM, USART_IT_RXNE);
	
	//EN_INT();
}



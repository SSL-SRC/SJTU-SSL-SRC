/********************************************************************************
********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:    Usart_ble.h         
* ��������:    
       ����ģ�鴮��ͨ��Э��ͷ�ļ�����
* �汾      ����       ʱ��          ״̬
* V1.0      sunlf      2015.10.14    �����ļ�
*****************************************************************************
*****************************************************************************/ 

#ifndef __USART_BLE_H
#define __USART_BLE_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"




#define UART_RX_BUF_SIZE		256
#define UART_TX_BUF_SIZE		256

#define UART_NUM				USART3

#define UART_BLE_ACK_MAX_PCKT_SIZE		25
#define UART_PCKT_RX_SIZE		25
#define UART_PCKT_TX_SIZE		25
#define UART_HM_BLE_ACK_L       7
#define UART_PACK_FRAME_HEAD 0xff

extern u8 buf_packet_ok[UART_PCKT_RX_SIZE];
extern u8 ble_ack_ok;
extern volatile char g_is_uart_get_a_packet ;  //���ڽ��յ�PC�����ݰ���־λ
extern volatile char g_is_uart_sent_a_packet ;  //���ڷ��͵�PC�����ݰ���־λ
extern volatile char g_is_uart_get_packet_num ;
extern volatile char g_is_uart_sent_packet_num ;



/***485ͨ������ö��***/
typedef enum
{
    BaudRate_9600=9600,
	BaudRate_19200=19200,
	BaudRate_115200=115200
}USART_BAUDRATE_EMUN;


void Init_com3(USART_BAUDRATE_EMUN baudrate);
void SendString_USART3(u8* str);
void SendChar_USART3(u8* buffer,u8 len);
 int get_uart_ble_ack_packet(u8 *buf);
 int get_uart_ble_packet(u8 *buf);



#endif //endif USART_485_H

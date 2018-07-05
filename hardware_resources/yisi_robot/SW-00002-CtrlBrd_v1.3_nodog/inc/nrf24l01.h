/******************************************************************************
 * nrf24l01.h - 
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
#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"
#include "nrf24l01_drv.h"

typedef struct _nRF24L01_buf
{
	int len;
	int pos;   //nrf接收或者发送数据包长度
	unsigned char buf[256];
}nRF24L01_buf;

typedef struct nRF24L01
{
	void (*init_dev)(void);
	int (*get_packet)(struct nRF24L01 *);
	int (*send_packet)(struct nRF24L01 *);
	nRF24L01_buf buf;
	unsigned int packet_error;
}nRF24L01;

extern nRF24L01 nRF24L01_dev;

/*Initialize the nRF24L01*/
void nrf24l01_init( void );

void start_nRF24L01_RX(void);

void set_receive_flag(void);
u8 get_receive_flag(void);
void clr_receive_flag(void);

int get_nRF24L01_packet(nRF24L01 *dev);
int send_nRF24L01_packet(nRF24L01 *dev);


#endif


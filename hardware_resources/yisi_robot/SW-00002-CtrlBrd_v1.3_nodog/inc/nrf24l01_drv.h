/*
================================================================================
Copyright(c)  Yihe Embedded Studio, All rights reserved.
              www.yh-mcu.com
              http://yh-mcu.taobao.com

File Name   : nRF24L01.c
Description : nRF24L01 low level operations and configurations.
Author      : ShawZhang,LiYong
Date        : 2009-4-17 9:08:36
Version     : 1.0
Compiler    : Any
Note        : None
================================================================================
*/
#ifndef _nRF24L01_DRV_H_
#define _nRF24L01_DRV_H_

#include "typedef.h"
#include "nRF24L01_Reg.h"
#include "gpio.h"

/*
================================================================================
============================Configurations and Options==========================
================================================================================
*/
#define DYNAMIC_PACKET      0 //1:DYNAMIC packet length, 0:fixed
#define REPEAT_CNT          0//0-15, repeat transmit count

/*
================================================================================
======================External IMPORT function list=============================
You MUST offers the following functions or macros for this c file:nRF24L01.c
1.L01_CSN_LOW( );                       //Pull low the CS line of SPI
2.L01_CSN_HIGH( );                      //Pull high the CS line if SPI
3.u8 SPI_ExchangeByte( u8 value );//SPI Exchange a byte
4.L01_CE_LOW( );
5.L01_CE_HIGH( );
================================================================================
*/

extern u8 nrf_spi_wr_rd(u8 input);

#define L01_CSN_LOW( )   NRF_NCS_LOW();
#define L01_CSN_HIGH( )  NRF_NCS_HIGH();
#define L01_CE_LOW( )    NRF_CE_LOW();
#define L01_CE_HIGH( )  NRF_CE_HIGH();

#define SPI_ExchangeByte(val) 	nrf_spi_wr_rd(val)

/*
================================================================================
=========================Local EXPORT function list=============================
======================Do NOT modify the following items=========================
================================================================================
*/
/*Read a single register of nRF24L01*/
u8 L01_ReadSingleReg( u8 Addr );

/*Read several registers of nRF24L01*/
void L01_ReadMultiReg( u8 StartAddr, u8 nBytes, u8 *pBuff );

/*Write a single byte to a register*/
void L01_WriteSingleReg( u8 Addr, u8 Value );

/*Read several registers of nRF24L01*/
void L01_WriteMultiReg( u8 StartAddr, u8 *pBuff, u8 Length );

/*Flush the TX buffer*/
void L01_FlushTX( void );

/*Flush the RX buffer*/
void L01_FlushRX( void );

/*reuse payload in tx fifo*/
void L01_ReuseTXPayload( void );

/* nop operation of nRF24L01 */
void L01_Nop( void );

/*Reuse the last transmitted payload*/
void L01_ReuseTXPayload( void );

/* read statu register of nRF24l01*/
u8 L01_ReadStatusReg( void );

/*L01 clear IRQ*/
#define IRQ_ALL  ( (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT) )
void L01_ClearIRQ( u8 IRQ_Source );

/*Read the IRQ source of nRF24L01+*/
u8 L01_ReadIRQSource( void );

/*Read the payload width of the top buffer of FIFO*/
u8 L01_ReadTopFIFOWidth( void );

/*Read the RX payload from internal buffer*/
u8 L01_ReadRXPayload( u8 *pBuff );

/*Write TX payload to a pipe and prx will return ack back*/
void L01_WriteTXPayload_Ack( u8 *pBuff, u8 nBytes );

/*write data in tx mode, and prx won't return ack back*/
void L01_WriteTXPayload_NoAck( u8 *Data, u8 Data_Length );

/*write data in tx fifo when rx mode*/
void L01_WriteRXPayload_InAck( u8 *pData, u8 Data_Length );

/*Write address for the own device*/
void L01_SetTXAddr( u8 *pAddr, u8 Addr_Length );

/*Write address for a RX pipe*/
void L01_SetRXAddr( u8 PipeNum, u8 *pAddr, u8 Addr_Length );

/*Set a hopping point for the device,must be 0-125*/
void L01_WriteHoppingPoint( u8 FreqPoint );

/*Set the device as TX or RX mode*/
typedef enum{ TX_MODE, RX_MODE }L01MD;
void L01_SetTRMode( L01MD mode );

/*Send the communication speed of the RF device*/
typedef enum{ SPD_250K, SPD_1M, SPD_2M  }L01SPD;
void L01_SetSpeed( L01SPD speed );

/*Send the transmitt power of the device*/
typedef enum{ P_F18DBM, P_F12DBM, P_F6DBM, P_0DBM }L01PWR;
void L01_SetPower( L01PWR power );


#endif//_nRF24L01_H_
/*
================================================================================
====================================End of file=================================
================================================================================
*/

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
#include "nRF24L01.h"

/*
================================================================================
Function : L01_ReadSingleReg( )
Description : Read a single register of nRF24L01
Input : -Addr, The address of the register
Output: The value read from the register
================================================================================
*/
u8 L01_ReadSingleReg( u8 Addr )
{
    u8 btmp;
    L01_CSN_LOW( );
    SPI_ExchangeByte( R_REGISTER | Addr );
    btmp = SPI_ExchangeByte( 0xFF );
    L01_CSN_HIGH( );
    return btmp;
}

/*
================================================================================
Function : L01_ReadMultiReg( )
Description : Read several registers of nRF24L01
Input : -StartAddr, The start address of the registers
        -nBytes, How many registers do you want to read
        -pBuff, The buffer to save the values
Output: None
================================================================================
*/
void L01_ReadMultiReg( u8 StartAddr, u8 nBytes, u8 *pBuff )
{
    u8 btmp;
    L01_CSN_LOW( );
    SPI_ExchangeByte( R_REGISTER | StartAddr );
    for( btmp = 0; btmp < nBytes; btmp ++ )
    {
        *( pBuff + btmp ) = SPI_ExchangeByte( 0xFF );
    }
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_WriteSingleReg( )
Description : Write a single byte to a register
Input : -Addr, The address of the register
        -Value, The value to be written
Output: None
================================================================================
*/
void L01_WriteSingleReg( u8 Addr, u8 Value )
{
    L01_CSN_LOW( );
    SPI_ExchangeByte( W_REGISTER | Addr );
    SPI_ExchangeByte( Value );
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_WriteMultiReg( )
Description : Read several registers of nRF24L01
Input : -StartAddr, The start address of the registers
        -pBuff, The buffer store the values
        -Length, How many registers do you want to write
Output: None
================================================================================
*/
void L01_WriteMultiReg( u8 StartAddr, u8 *pBuff, u8 Length )
{
    u8 i;
    L01_CSN_LOW( );
    SPI_ExchangeByte( W_REGISTER | StartAddr );
    for( i = 0; i < Length; i ++ )
    {
        SPI_ExchangeByte( *( pBuff + i ) );
    }
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_FlushTX( )
Description : Flush the TX buffer
Input : None
Output: None
================================================================================
*/
void L01_FlushTX( void )
{
    L01_CSN_LOW( );
    SPI_ExchangeByte( FLUSH_TX );
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_FlushRX( )
Description : Flush the RX buffer
Input : None
Output: None
================================================================================
*/
void L01_FlushRX( void )
{
    L01_CSN_LOW( );
    SPI_ExchangeByte( FLUSH_RX );
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_ReuseTXPayload( )
Description : Reuse the last transmitted payload
Input : None
Output: None
================================================================================
*/
void L01_ReuseTXPayload( void )
{
    L01_CSN_LOW( );
    SPI_ExchangeByte( REUSE_TX_PL );
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_Nop( )
Description : nop operation of nRF24L01
Input : None
Output: None
================================================================================
*/
void L01_Nop( void )
{
    L01_CSN_LOW( );
    SPI_ExchangeByte( L01_NOP );
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_ReadStatusReg( )
Description : Read statu register of nRF24L01
Input : None
Output: Statu register of nRF24L01
================================================================================
*/
u8 L01_ReadStatusReg( void )
{
    u8 Status;
    L01_CSN_LOW( );
    Status = SPI_ExchangeByte( R_REGISTER + L01REG_STATUS );
    L01_CSN_HIGH( );
    return Status;
}

/*
================================================================================
Function : L01_ClearIRQ( )
Description : Clear IRQ cuased by nRF24L01
Input : None
Output: None
================================================================================
*/
void L01_ClearIRQ( u8 IRQ_Source )
{
    u8 btmp = 0;

    IRQ_Source &= ( 1<<RX_DR ) | ( 1<<TX_DS ) | ( 1<<MAX_RT );
    btmp = L01_ReadStatusReg( );
    L01_CSN_LOW( );
    SPI_ExchangeByte( W_REGISTER + L01REG_STATUS );
    SPI_ExchangeByte( IRQ_Source | btmp );
    L01_CSN_HIGH( );
    L01_ReadStatusReg( );
}

/*
================================================================================
Function : L01_ReadIRQSource( )
Description : Read the IRQ source of nRF24L01+
Input : None
Output: IRQ source mask code
================================================================================
*/
u8 L01_ReadIRQSource( void )
{
    return ( L01_ReadStatusReg( ) & ( ( 1<<RX_DR ) | ( 1<<TX_DS ) | ( 1<<MAX_RT ) ) );
}

/*
================================================================================
Function : L01_ReadTopFIFOWidth( )
Description : Read the payload width of the top buffer of FIFO
Input : None
Output: The width of the pipe buffer
================================================================================
*/
u8 L01_ReadTopFIFOWidth( void )
{
    u8 btmp;
    L01_CSN_LOW( );
    SPI_ExchangeByte( R_RX_PL_WID );
    btmp = SPI_ExchangeByte( 0xFF );
    L01_CSN_HIGH( );
    return btmp;
}

/*
================================================================================
Function : L01_ReadRXPayload( )
Description : Read the RX payload from internal buffer
Input : -pBuff, buffer to store the data
Output: The length of data read
================================================================================
*/
u8 L01_ReadRXPayload( u8 *pBuff )
{
    u8 width, PipeNum;
    //PipeNum = ( L01_ReadSingleReg( L01REG_STATUS ) >> 1 ) & 0x07;
    width = L01_ReadTopFIFOWidth( );
	if(width > 32)
	{
		/* the data wdith is error */
		L01_FlushRX( );
		return 33;
	}
	else if(width == 0)
	{
		return 0;
	}

    L01_CSN_LOW( );
    SPI_ExchangeByte( R_RX_PAYLOAD );
    for( PipeNum = 0; PipeNum < width; PipeNum ++ )
    {
        *( pBuff + PipeNum ) = SPI_ExchangeByte( 0xFF );
    }
    L01_CSN_HIGH( );
    L01_FlushRX( );
    return width;
}

/*
================================================================================
Function : L01_WriteTXPayload( )
Description : Write TX payload to a pipe and prx will return ack back
Input : -PipeNum, number of the pipe
        -pBuff, A buffer stores the data
        -nBytes, How many bytes to be wrote to
Output: None
================================================================================
*/
void L01_WriteTXPayload_Ack( u8 *pBuff, u8 nBytes )
{
    u8 btmp;
    u8 length = ( nBytes > 32 ) ? 32 : nBytes;

    L01_FlushTX( );
    L01_CSN_LOW( );
    SPI_ExchangeByte( W_TX_PAYLOAD );
    for( btmp = 0; btmp < length; btmp ++ )
    {
        SPI_ExchangeByte( *( pBuff + btmp ) );
    }
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_WritePayload_NoAck( )
Description : write data in tx mode, and prx won't return ack back
Input : -Data, A buffer stores the address data
        -Data_Length, How many bytes of the data buff
Output: None
================================================================================
*/
void L01_WriteTXPayload_NoAck( u8 *Data, u8 Data_Length )
{
    if( Data_Length > 32 || Data_Length == 0 )
    {
        return ;
    }
    L01_CSN_LOW( );
    SPI_ExchangeByte( W_TX_PAYLOAD_NOACK );
    while( Data_Length-- )
    {
        SPI_ExchangeByte( *Data++ );
    }
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_WritePayload_InAck( )
Description : write data in tx fifo when rx mode
Input : -Data, A buffer stores the address data
        -Data_Length, How many bytes of the data buff
Output: None
================================================================================
*/
void L01_WriteRXPayload_InAck( u8 *pData, u8 Data_Length )
{
    u8 length = ( Data_Length > 32 ) ? 32 : Data_Length;
    u8 btmp;

    L01_CSN_LOW( );
    SPI_ExchangeByte( W_ACK_PAYLOAD );
    for( btmp = 0; btmp < length; btmp ++ )
    {
        SPI_ExchangeByte( *( pData + btmp ) );
    }
    L01_CSN_HIGH( );
}

/*
================================================================================
Function : L01_SetTXAddr( )
Description : Write address for the own device
Input : -pAddr, A buffer stores the address data
        -Addr_Length, How many bytes of the address
Output: None
================================================================================
*/
void L01_SetTXAddr( u8 *pAddr, u8 Addr_Length )
{
    u8 Length = ( Addr_Length > 5 ) ? 5 : Addr_Length;
    L01_WriteMultiReg( L01REG_TX_ADDR, pAddr, Length );
}

/*
================================================================================
Function : L01_SetRXAddr( )
Description : Write address for a RX pipe
Input : -PipeNum, number of the pipe
        -pAddr, A buffer stores the address data
        -Addr_Length, How many bytes of the address
Output: None
================================================================================
*/
void L01_SetRXAddr( u8 PipeNum, u8 *pAddr, u8 Addr_Length )
{
    u8 Length = ( Addr_Length > 5 ) ? 5 : Addr_Length;
    u8 pipe = ( PipeNum > 5 ) ? 5 : PipeNum;

    L01_WriteMultiReg( L01REG_RX_ADDR_P0 + pipe, pAddr, Length );
}

/*
================================================================================
Function : L01_SetSpeed )
Description : Send the communication speed of the RF device
Input :    speed,
Output: None
================================================================================
*/
void L01_SetSpeed( L01SPD speed )
{
	u8 btmp = L01_ReadSingleReg( L01REG_RF_SETUP );

	btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	if( speed == SPD_250K )		//250K
	{
		btmp |= ( 1<<5 );
	}
	else if( speed == SPD_1M )   //1M
	{
   		btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	}
	else if( speed == SPD_2M )   //2M
	{
		btmp |= ( 1<<3 );
	}

	L01_WriteSingleReg( L01REG_RF_SETUP, btmp );
}

/*
================================================================================
Function : L01_SetPower )
Description : Send the transmitt power of the device
Input :    power
Output: None
================================================================================
*/
void L01_SetPower( L01PWR power )
{
    u8 btmp = L01_ReadSingleReg( L01REG_RF_SETUP ) & ~0x07;
    switch( power )
    {
        case P_F18DBM:
            btmp |= PWR_18DB;
            break;
        case P_F12DBM:
            btmp |= PWR_12DB;
            break;
        case P_F6DBM:
            btmp |= PWR_6DB;
            break;
        case P_0DBM:
            btmp |= PWR_0DB;
            break;
        default:
            break;
    }
    L01_WriteSingleReg( L01REG_RF_SETUP, btmp );
}

/*
================================================================================
Function : L01_WriteHoppingPoint( )
Description : Set frequency for the device,must be 0-125
Input : -FreqPoint, The hopping point
Output: None
================================================================================
*/
void L01_WriteHoppingPoint( u8 FreqPoint )
{
    L01_WriteSingleReg( L01REG_RF_CH, FreqPoint & 0x7F );
}

/*
================================================================================
Function : L01_SetTRMode( )
Description : Set the device as TX or RX mode
Input : -mode, the mode to be set
            @TX_MODE, TX mode
            @RX_MODE, RX mode
Output: None
================================================================================
*/
void L01_SetTRMode( L01MD mode )
{
    u8 controlreg = L01_ReadSingleReg( L01REG_CONFIG );
    if( mode == TX_MODE )       { controlreg &= ~( 1<<PRIM_RX ); }
    else if( mode == RX_MODE )  { controlreg |= ( 1<<PRIM_RX ); }

    L01_WriteSingleReg( L01REG_CONFIG, controlreg );
}

/*
================================================================================
====================================End of file=================================
================================================================================
*/

/******************************************************************************
 * i2c.h - I2C operation function.
 * 
 * Copyright 2008-2011 Shouxian Chen.
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0   2011/12/14, Shouxian Chen create this file
 * 
 ******************************************************************************/

#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "timer.h"

/* IO simluation I2C bus, operation define */
#define SCL_H       GPIOB->BSRRL = GPIO_Pin_10 //PB10

#define SCL_L       GPIOB->BSRRH = GPIO_Pin_10
    
#define SDA_H       GPIOB->BSRRL = GPIO_Pin_11	//PB11

#define SDA_L       GPIOB->BSRRH = GPIO_Pin_11

#define SDA_RD(val) (val = GPIOB->IDR & GPIO_Pin_11)

#define i2c_delay()		wait_us(10)

/* i2c eeprom parameter define */
#define PAGE_WR_SIZE				8
#define HIGH_SZ_PAGE_WR_SIZE		32
#define EEPROM_DEV_ADDR				0xa0

void I2C_MPU6050_GPIO_Config(void);
void I2C_MPU6050_Mode_Config(void);

uint8_t I2C_MPU6050_ByteRead(uint8_t REG_Address);

void init_i2c(void);
u8 i2c_m_send_dat(u8 dev_addr, u8 *buf, u8 cnt);
u8 i2c_m_read_dat(u8 dev_addr, u8 *buf, u8 cnt);
u8 m_probe_i2c_dev(u8 dev_addr);
void low_size_eeprom_rd(void *dat_buf, u16 addr, u32 count);
void low_size_eeprom_wr(void *dat_buf, u16 addr, u32 count);
u8 i2c_m_rd_byte(u8 dev_addr, u8 reg_addr, u8 *dat);
u8 i2c_m_wr_byte(u8 dev_addr, u8 reg_addr, u8 dat);

void high_size_eeprom_rd(void *dat_buf, u16 addr, u32 count);
void high_size_eeprom_wr(void *dat_buf, u16 addr, u32 count);



#endif



/******************************************************************************
* misc.h - 
* 
* Copyright 2008-2015 Shouxian Chen.
* 
* DESCRIPTION: - 
* 
* modification history
* --------------------
* v1.0   2015/09/10, Shouxian Chen create this file
* 
******************************************************************************/
#ifndef __MISC_H__
#define __MISC_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"

void set_heart_led(u8 r, u8 g, u8 b, u8 bright);
void heart_beat(void);

void shoot_off(void);
void chip_off(void);
void shoot_on(u32 value);
void chip_on(u32 value);
void init_shoot_chip_timer(void);

void init_dribbler(void);
void set_dribbler(u16 value, u8 dir);

int is_ball_detected(void);
void init_ir_pwm(void);
void start_ir_pwm(void);
void stop_ir_pwm(void);

void init_ball_dect_irq(void);
void start_ball_dect_int(void);
void stop_ball_dect_int(void);

long N2RPM(long speed);
long RPM2N(long rpm);
long V2N(float vel);
float N2V(long n);

void init_adc(void);
u8 get_cap_v(void);
u8 get_bat_v(void);

u8 nrf_spi_wr_rd(u8 input);
void init_nf24l01_spi(void);

void read_dip_sw(u8 *freq, u8 *num, u8 *mode);

int is_infra_broken(void);
int is_power_low(void);



#endif
 

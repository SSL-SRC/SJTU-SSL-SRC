/******************************************************************************
 * motor.h - 
 * 
 * Copyright 2008-2013 Shouxian Chen.
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0   2015/08/08, Shouxian Chen create this file
 * 
 ******************************************************************************/
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"

void update_motor(void);
void init_motor(void);
void start_motor(void);
void stop_motor(void);
void get_motor_speed(int *speed);

void start_encoder(void);


#endif

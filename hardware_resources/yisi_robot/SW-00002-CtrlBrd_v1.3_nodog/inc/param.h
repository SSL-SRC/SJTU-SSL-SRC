/******************************************************************************
 * param.h - 
 * 
 * Copyright 2008-2015 Shouxian Chen.
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0   2015/09/15, Shouxian Chen create this file
 * 
 ******************************************************************************/
#ifndef __PARAM_H__
#define __PARAM_H__

#include "typedef.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "cfg.h"

#define MAGIC_NUM	0x5a

typedef struct _parameter
{
	u8 dat[PARAM_LEN];
	u8 magic;
	u8 chk_sum;
}param_t;


int load_param(param_t *param);
int load_blue_param(void);

#endif


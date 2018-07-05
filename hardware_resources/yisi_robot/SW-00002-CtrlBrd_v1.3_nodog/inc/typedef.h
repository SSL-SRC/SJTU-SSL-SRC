/********************************************************************************
********************************************************************************
* Ȩ��:	�����Ͻ������˹ɷ����޹�˾
* �ļ���:	  typedef.h		
* ��������:	
*		    ���Ͷ���
* �汾 	 ����		ʱ��		  ״̬
* V1.0 	 chenshouxian	2015.08.08	�����ļ�
*****************************************************************************
*****************************************************************************/


#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;
typedef unsigned long long U64,u64;
typedef unsigned volatile long long VU64,vu64;

typedef char S8;
typedef short S16;
typedef long S32;	
typedef long long S64,s64;

typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned short USHORT;

#define NULL	0

/* enable and disable global int */
#define EN_INT() __enable_fiq()
#define DIS_INT() __disable_fiq()

//#define NOP()	__asm("nop;")
#define NOP()	__nop()


#endif



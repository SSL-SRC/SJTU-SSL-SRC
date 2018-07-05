/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:	 cpuid_des.c	   
* 功能描述:    
		 cpuid DES加密算法
* 版本		作者	   时间 		 状态
* V1.0		sunlifeng   2016.4.11   创建文件
*****************************************************************************
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"

#define ZERO_STRUCT(x) memset((char *)&(x), 0, sizeof(x))


//des part
static const uint8_t perm1[56] = {
			56, 48, 40, 32, 24, 16,  8,
			 0, 57, 49, 41, 33, 25, 17,
			 9,  1, 58, 50, 42, 34, 26,
			18, 10,  2, 59, 51, 43, 35,
			62, 54, 46, 38, 30, 22, 14,
			 6, 61, 53, 45, 37, 29, 21,
			13,  5, 60, 52, 44, 36, 28,
			20, 12,  4, 27, 19, 11,  3};

static const uint8_t perm2[48] = {
			13, 16, 10, 23,  0,  4,
			 2, 27, 14,  5, 20,  9,
			22, 18, 11,  3, 25,  7,
			15,  6, 26, 19, 12,  1,
			40, 51, 30, 36, 46, 54,
			29, 39, 50, 44, 32, 47,
			43, 48, 38, 55, 33, 52,
			45, 41, 49, 35, 28, 31};

static const uint8_t perm3[64] = {
			57, 49, 41, 33, 25, 17,  9,  1,
			59, 51, 43, 35, 27, 19, 11,  3,
			61, 53, 45, 37, 29, 21, 13,  5,
			63, 55, 47, 39, 31, 23, 15,  7,
			56, 48, 40, 32, 24, 16,  8,  0,
			58, 50, 42, 34, 26, 18, 10,  2,
			60, 52, 44, 36, 28, 20, 12,  4,
			62, 54, 46, 38, 30, 22, 14,  6};

static const uint8_t perm4[48] = {
			31,  0,  1,  2,  3,  4,
			 3,  4,  5,  6,  7,  8,
			 7,  8,  9, 10, 11, 12,
			11, 12, 13, 14, 15, 16,
			15, 16, 17, 18, 19, 20,
			19, 20, 21, 22, 23, 24,
			23, 24, 25, 26, 27, 28,
			27, 28, 29, 30, 31,  0};

static const uint8_t perm5[32] = {
			15,  6, 19, 20,
			28, 11, 27, 16,
			 0, 14, 22, 25,
			 4, 17, 30,  9,
			 1,  7, 23, 13,
			31, 26,  2,  8,
			18, 12, 29,  5,
			21, 10,  3, 24};

/* n <-- (((~n)&1)<<5)|((n&6)<<2)|((~(n>>3))&7) */
static const uint8_t perm6[64] = {
			39,  7, 47, 15, 55, 23, 63, 31,
			38,  6, 46, 14, 54, 22, 62, 30,
			37,  5, 45, 13, 53, 21, 61, 29,
			36,  4, 44, 12, 52, 20, 60, 28,
			35,  3, 43, 11, 51, 19, 59, 27,
			34,  2, 42, 10, 50, 18, 58, 26,
			33,  1, 41,  9, 49, 17, 57, 25,
			32,  0, 40,  8, 48, 16, 56, 24};


static const uint8_t sc[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

static const uint8_t sbox[8][4][16] = {
	{{14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
	 {0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
	 {4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
	 {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}},

	{{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
	 {3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
	 {0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
	 {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}},

	{{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
	 {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
	 {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
	 {1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}},

	{{7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
	 {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
	 {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
	 {3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}},

	{{2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
	 {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
	 {4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
	 {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}},

	{{12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
	 {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
	 {9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
	 {4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}},

	{{4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
	 {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
	 {1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
	 {6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}},

	{{13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
	 {1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
	 {7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
	 {2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}}};

static void permute(char *out, const char *in, const uint8_t *p, int n)
{
	int i;
	for (i=0;i<n;i++)
	{
	   out[i] = in[p[i]];
	}
		
}

static void lshift(char *d, int count, int n)
{
	char out[64];
	int i;
	for (i=0;i<n;i++)
		out[i] = d[(i+count)%n];
	for (i=0;i<n;i++)
		d[i] = out[i];
}

static void concat(char *out, char *in1, char *in2, int l1, int l2)
{
	while (l1--)
		*out++ = *in1++;
	while (l2--)
		*out++ = *in2++;
}

static void xor(char *out, char *in1, char *in2, int n)
{
	int i;
	for (i=0;i<n;i++)
	 out[i] = in1[i] ^ in2[i];
}

static void dohash(char *out, char *in, char *key, int forw)
{
	int i, j, k;
	char pk1[56];
	char c[28];
	char d[28];
	char cd[56];
	char ki[16][48];
	char pd1[64];
	char l[32], r[32];
	char rl[64];

	permute(pk1, key, perm1, 56);

	for (i=0;i<28;i++)
		c[i] = pk1[i];
	for (i=0;i<28;i++)
		d[i] = pk1[i+28];

	for (i=0;i<16;i++) {
		lshift(c, sc[i], 28);
		lshift(d, sc[i], 28);

		concat(cd, c, d, 28, 28); 
		permute(ki[i], cd, perm2, 48); 
	}

	permute(pd1, in, perm3, 64);

	for (j=0;j<32;j++) {
		l[j] = pd1[j];
		r[j] = pd1[j+32];
	}

	for (i=0;i<16;i++) {
		char er[48];
		char erk[48];
		char b[8][6];
		char cb[32];
		char pcb[32];
		char r2[32];

		permute(er, r, perm4, 48);

		xor(erk, er, ki[forw ? i : 15 - i], 48);

		for (j=0;j<8;j++)
			for (k=0;k<6;k++)
				b[j][k] = erk[j*6 + k];

		for (j=0;j<8;j++) {
			int m, n;
			m = (b[j][0]<<1) | b[j][5];

			n = (b[j][1]<<3) | (b[j][2]<<2) | (b[j][3]<<1) | b[j][4]; 

			for (k=0;k<4;k++) 
				b[j][k] = (sbox[j][m][n] & (1<<(3-k)))?1:0; 
		}

		for (j=0;j<8;j++)
			for (k=0;k<4;k++)
				cb[j*4+k] = b[j][k];
		permute(pcb, cb, perm5, 32);

		xor(r2, l, pcb, 32);

		for (j=0;j<32;j++)
			l[j] = r[j];

		for (j=0;j<32;j++)
			r[j] = r2[j];
	}

	concat(rl, r, l, 32, 32);

	permute(out, rl, perm6, 64);
}

static void str_to_key(const uint8_t *str,uint8_t *key)
{
	int i;

	key[0] = str[0]>>1;
	key[1] = ((str[0]&0x01)<<6) | (str[1]>>2);
	key[2] = ((str[1]&0x03)<<5) | (str[2]>>3);
	key[3] = ((str[2]&0x07)<<4) | (str[3]>>4);
	key[4] = ((str[3]&0x0F)<<3) | (str[4]>>5);
	key[5] = ((str[4]&0x1F)<<2) | (str[5]>>6);
	key[6] = ((str[5]&0x3F)<<1) | (str[6]>>7);
	key[7] = str[6]&0x7F;
	for (i=0;i<8;i++) {
		key[i] = (key[i]<<1);
	}
}

static void key_to_str(const uint8_t *key,uint8_t *str)
{
	str[0] = (key[0]&0xfe) | ((key[1]&0x80)>>7);
	str[1] = ((key[1]&0x7e)<<1) | ((key[2]&0xc0)>>6);
	str[2] = ((key[2]&0x3e)<<2) | ((key[3]&0xe0)>>5);
	str[3] = ((key[3]&0x1e)<<3) | ((key[4]&0xf0)>>4);
	str[4] = ((key[4]&0x0e)<<4) | ((key[5]&0xf8)>>3);
	str[5] = ((key[5]&0x06)<<5) | ((key[6]&0xfc)>>2);
	str[6] = ((key[6]&0x02)<<6) | ((key[7]&0xfe)>>1);
}

/*
  basic des crypt using a 56 bit (7 byte) key
*/
void des_crypt56(uint8_t out[8], const uint8_t in[8], const uint8_t key[7], int forw)
{
	int i;
	char outb[64];
	char inb[64];
	char keyb[64];
	uint8_t key2[8];

	str_to_key(key, key2);

	for (i=0;i<64;i++) {
		inb[i] = (in[i/8] & (1<<(7-(i%8)))) ? 1 : 0;
		keyb[i] = (key2[i/8] & (1<<(7-(i%8)))) ? 1 : 0;
		outb[i] = 0;
	}

	dohash(outb, inb, keyb, forw);

	for (i=0;i<8;i++) {
		out[i] = 0;
	}

	for (i=0;i<64;i++) {
		if (outb[i])
			out[i/8] |= (1<<(7-(i%8)));
	}
}


/*******************************************************************************
* Function Name   : encrypt_CPUID
* Description	  : EDS加密CPUID算法
*					
* Input		      : uint8_t *org 传入原始数据  int len 传入的原始数据的长度  CPUID共12byte
* Output		  : uint8_t *dest 保存加密后结果，空间要求至少8字节
* Return		  : None.
*******************************************************************************/
void encrypt_CPUID(uint8_t *dest, const uint8_t *org, int len)
{
	uint8_t o1[16];
	uint8_t key[16];
	uint8_t p1[16];

	ZERO_STRUCT(o1);
	ZERO_STRUCT(key);
	ZERO_STRUCT(p1);
	if(len>7) 
	{
		memcpy(o1, org, 7);
		if (len<15)
			memcpy(key, org+7, len-7);
		else
			memcpy(key, org+7, 8);
	} 
	else 
	{
		memcpy(o1, org, len);
		memcpy(key, "hznjrobot", 8);
	}
	des_crypt56(p1, key, o1, 1);
	memcpy(dest, p1, 8);
}

/*
 * 将字节数据转换成字符串，只处理8个字节
 * dest - 保存转换后的字符串，空间要求至少32个字节
 * org  - 传入原始字节数据
 */
void byte2str(char *dest, const uint8_t *org)
{
	int i;
	for (i=0; i<8; i++) {
#ifdef WIN32
		if (i==0)
			sprintf_s(dest, 4, "%02x", (unsigned int)org[i]);
		else
			sprintf_s(dest+i*3-1, 4, "-%02x", (unsigned int)org[i]);
#else
		if (i==0)
			sprintf(dest, "%02x", (unsigned int)org[i]);
		else
			sprintf(dest+i*3-1, "-%02x", (unsigned int)org[i]);
#endif
	}
}

/*
 * 将字符串转换成字节数据，只处理8个字节
 * dest - 保存转换后的字节数据，空间要求至少8个字节
 * org  - 传入原始字符串
 */
void str2byte(uint8_t *dest, const char *org)
{
	uint8_t o1[8];
	ZERO_STRUCT(o1);
#ifdef WIN32
	sscanf_s(org, "%x-%x-%x-%x-%x-%x-%x-%x", &o1[0], &o1[1], &o1[2], &o1[3], &o1[4], &o1[5], &o1[6], &o1[7]);
#else
	sscanf(org, "%x-%x-%x-%x-%x-%x-%x-%x", &o1[0], &o1[1], &o1[2], &o1[3], &o1[4], &o1[5], &o1[6], &o1[7]);
#endif
	memcpy(dest, o1, 8);
}


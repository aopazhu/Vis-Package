#ifndef __PRINT_H_
#define __PRINT_H_

#include "rtc.h"
#include "stdio.h"

#define  	Print_CLK          RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC

#define		Print_C_groud		GPIOB
#define		Print_BUSY			GPIO_Pin_0
#define		Print_STB				GPIO_Pin_1
#define		Print_D_group		GPIOC
#define		Print_Data			GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7

#define  	PRT_Data(x)		{Print_D_group->BSRR = (x&0x00ff);Print_D_group->BRR=((~x)&0x00ff);}

#define   PRT_STB(x)		x ? GPIO_SetBits(Print_C_groud,Print_STB):   GPIO_ResetBits(Print_C_groud,Print_STB)
#define 	PRT_BUSY 			PBin(0)

typedef struct _print_st
{
	char	print_buf[256];
	u8	rd_ptr;
	u8	wr_ptr;
}print_st,PRINT_ST_PTR;


void PRINT_InitPort(void);
void PRINT_Write_Data(u8 data);
void PRINT_Init(void);
u8 PRINT_Str(u8* tmp_str,u8 len);
void PRINT_test(void);
void PRINT_module(_calendar_obj tmp_rtc,u8 r_t,u8 s_t,float wend,float tmp_vis,float tmp_pct);
#endif

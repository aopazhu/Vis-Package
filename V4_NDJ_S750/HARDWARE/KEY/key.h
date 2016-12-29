#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "stm32f10x_exti.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 

// 
//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP
 
#define LINT0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define LINT1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
 
 
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读取按键2
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//读取按键3 
#define KEY4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)//读取按键4 
#define KEY5  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//读取按键3 
#define KEY6  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//读取按键4

#define KEY_UP 		1
#define KEY_DOWN	4
#define KEY_LEFT	2
#define KEY_RIGHT	5
#define KEY_ENTER	6
#define KEY_BACK	3

// const static u32 VisRg[5][3]={
// {      0,     0,  100},
// {      0,     0, 1000},
// {      0,     0,10000},
// {      0,100000,    0},
// {1000000,     0,    0}
// };

void KEY_Init(void);//IO初始化
void EXTIX_Init(void);//外部中断初始化		
void KEY_Scan(void);  	//按键扫描函数		
void display_JDT(void);
void meas_DP(void);
u8 up_down(void);
u8 left_right(void);
#endif

#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "stm32f10x_exti.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 

// 
//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP
 
#define LINT0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define LINT1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
 
 
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//��ȡ����2
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//��ȡ����3 
#define KEY4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)//��ȡ����4 
#define KEY5  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//��ȡ����3 
#define KEY6  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//��ȡ����4

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

void KEY_Init(void);//IO��ʼ��
void EXTIX_Init(void);//�ⲿ�жϳ�ʼ��		
void KEY_Scan(void);  	//����ɨ�躯��		
void display_JDT(void);
void meas_DP(void);
u8 up_down(void);
u8 left_right(void);
#endif

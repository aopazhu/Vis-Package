#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "12864.h"
#include "rtc.h"
#include "at24cxx.h" 	
#include "timer.h"
#include "print.h"
#include "thb6128.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_tim.h"
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
#define	autoAdjust 1.5
#define	fulltmp 800

#define		SPEED_SUM		3
#define		SPEED_AUTO		0

#if	NDJ_4S
	#define		S_BOTTOM		S_7_5
	#define		S_TOP			S_750
#elif	NDJ_7S
	#define		S_BOTTOM		S_7_5
	#define		S_TOP			S_750
#else
	#define		S_BOTTOM		S_7_5
	#define		S_TOP			S_750
#endif

#define	AVERAGE 0

extern u8	english_flag;
extern u8 meas_flag;
extern u8 few_flag;
extern u8 r_type;
extern u8 s_type;
extern u8 o_type;
extern u8 c_type;
extern u8 MUEN_ST;
extern u8 ITEM_ST;
extern u8 rtc_set;
extern u8 motor_start,motor_trig;
extern uint16_t start_cnt;
extern unsigned int lint_cnt;
u8 c_sel=0,step_JDT=0,step_autoZ=0,autoZ_flag=0,auto_meas=0,DV_sel=0,D_sel=0;
u32 V_Range;
extern float Angle,Vis,temper;

extern print_st print_FIFO;

extern float	V_slow;
extern double	K_slow0;
extern double	K_slow;

float zoretmp,zoretmp1;
float ktmp,ktmp1;

float	P_Angle=0.1;
float	KG_Angle;
float	Y_Angle;
unsigned int	clint_cnt;
//const unsigned int sum_cnt[9]={525000000,175000000,87500000,35000000,17500000,8750000,4375000,1750000,875000};//=(315/360)*(60/Vs)*1000000
const unsigned int sum_cnt[SPEED_SUM]={7000000,700000,70000};//=(315/360)*(60/Vs)*1000000	

const static u32 VisRg[7][3]={
{      0,      0,     10},
{      0,      0,     20},
{      0,      0,     40},
{      0,      0,     50},
{      0,      0,    100},
{      0,      0,   1000},
{1000000, 100000,  10000}
};

u8	trig_flag=0;
u8	single_flag=0;
u8	r_tmp=0;
u8	s_tmp=0;

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化 按键 上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTB,PORTC时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IPD; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB_8
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB_8

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOB_8	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.8
}


void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    KEY_Init();	 //	按键端口初始化

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	//GPIOB.4	  中断线以及中断初始化配置 下降沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);

	//GPIOB.5	  中断线以及中断初始化配置 下降沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;	
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_Init(&EXTI_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能按键  所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键  所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 

}

void EXTI4_IRQHandler(void)
{
	unsigned int	tmp_tim=TIM3_Get_Value();
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
			EXTI_ClearITPendingBit(EXTI_Line4); 
			//if((LINT0==0)&&(single_flag==0))
			if(LINT0==0)
			{
				if((tmp_tim-lint_cnt)>(sum_cnt[s_type]>>1))
				{
						lint_cnt = tmp_tim;
						single_flag=1;
				}
			}
	}	
}

//外部中断0服务程序 
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5); 
		if(LINT1==0)
		{
			if(single_flag==1)
			{	
				clint_cnt=TIM3_Get_Value();
				clint_cnt-=lint_cnt;
				single_flag++;
				meas_flag=1;		
				trig_flag=1;
			}
		}		
	}	
	EXTI_ClearITPendingBit(EXTI_Line5); 
	EXTI_ClearITPendingBit(EXTI_Line8);  //清除LINE8上的中断标志位  
	EXTI_ClearITPendingBit(EXTI_Line9);  //清除LINE9上的中断标志位  
}
 
//外部中断2服务程序
void EXTI15_10_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line13);  //清除LINE13上的中断标志位  
	EXTI_ClearITPendingBit(EXTI_Line14);  //清除LINE14上的中断标志位  
	EXTI_ClearITPendingBit(EXTI_Line15);  //清除LINE15上的中断标志位  	
}

void KEY_Scan(void)
{	 
	u8 keytmp=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////	
	if(KEY1==0)	 //up按键
	{				 
		delay_ms(10);
		if(KEY1!=0)
		{	return;}
		switch(MUEN_ST)
		{
				case 0:
					if(up_down())
					{
							MUEN_ST=5;
							r_tmp=1;
							s_tmp=0;
#if	NDJ_4S|NDJ_7S
							if(s_tmp<S_BOTTOM)
							{	s_tmp=S_BOTTOM;}
#endif							
							if(s_tmp>S_TOP)
							{	s_tmp=S_TOP;}
							zoretmp=Read_Target(Zero,s_tmp);
							zoretmp1=zoretmp;
							LCD12864_Zname();
							LCD12864_Ztype(s_tmp,zoretmp);
					}					
					break;
				case 1:				
					ITEM_ST=(ITEM_ST+3)%4;
					LCD12864_ClrItem(ITEM_ST,1);	
				
					if(up_down())
					{
							
							MUEN_ST=10;
							P_Angle=0.1;
							step_autoZ=0;
							autoZ_flag=1;					
						motor_start=1;
							motor_trig=1;
							step_JDT=0;//进度条进度
							trig_flag=0;
							s_type=S_BOTTOM;
							zoretmp=0;
							ktmp=1;
							LCD12864_Zname();
							LCD12864_Ztype(s_type,zoretmp);
							THB6128_Turn_On(s_type);
					}
					break;
				case 2:
					switch(ITEM_ST)
					{
							case 0:
								
								break;
							case 1:
								
								break;
							case 2:
								
								break;
							case 3:
								if(c_type)
								{
									rtc_set=1;
									switch(c_sel)
									{
											case 0:
												if(english_flag==0)
												{
													calendar.w_year++;
													if(calendar.w_year==2100)
													{	calendar.w_year=2015;}
													LCDputvalue((float)calendar.w_year,1,42,0,4,1);
												}
												else
												{
													calendar.w_date++;
													switch(calendar.w_month)
													{
															case 1:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 2:	
																if((calendar.w_year%4)||((calendar.w_year%100==0)&&(calendar.w_year%400)))
																{
																	if(calendar.w_date==29)
																	{	calendar.w_date=1;}
																}
																else
																{
																	if(calendar.w_date==30)
																	{	calendar.w_date=1;}
																}
																break;
															case 3:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 4:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 5:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 6:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 7:
															case 8:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 9:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 10:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 11:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 12:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															default:break;	
													}
													LCDputvalue((float)calendar.w_date,1,42,0,2,1);
												}
												break;
											case 1:	
												calendar.w_month++;
												if(calendar.w_month==13)
												{	calendar.w_month=1;}
												if(english_flag==0)
												{
													LCDputvalue((float)calendar.w_month,1,70,0,2,1);
												}
												else
												{
													LCDputvalue((float)calendar.w_month,1,64,0,2,1);
												}
												break;
											case 2:
												if(english_flag==0)
												{
													calendar.w_date++;
													switch(calendar.w_month)
													{
															case 1:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 2:	
																if((calendar.w_year%4)||((calendar.w_year%100==0)&&(calendar.w_year%400)))
																{
																	if(calendar.w_date==29)
																	{	calendar.w_date=1;}
																}
																else
																{
																	if(calendar.w_date==30)
																	{	calendar.w_date=1;}
																}
																break;
															case 3:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 4:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 5:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 6:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 7:
															case 8:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 9:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 10:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															case 11:
																if(calendar.w_date==31)
																{	calendar.w_date=1;}
																break;
															case 12:
																if(calendar.w_date==32)
																{	calendar.w_date=1;}
																break;
															default:break;	
													}
													LCDputvalue((float)calendar.w_date,1,98,0,2,1);
												}
												else
												{
													calendar.w_year++;
													if(calendar.w_year==2100)
													{	calendar.w_year=2015;}
													LCDputvalue((float)calendar.w_year,1,99,0,4,1);
												}
												break;	
											case 3:
												calendar.hour++;//calendar.w_hour=calendar.w_hour%24;
												if(calendar.hour==24)
												{	calendar.hour=0; }
												LCDputvalue((float)calendar.hour,4,49,0,2,1);//LCD12864_value(4,32,calendar.hour,1);
												break;
											case 4:
												calendar.min++;
												calendar.min=calendar.min%60;
												LCDputvalue((float)calendar.min,4,71,0,2,1);//LCD12864_value(4,56,calendar.min,1);
												break;
											case 5:
												calendar.sec++;
												calendar.sec=calendar.sec%60;
												LCDputvalue((float)calendar.sec,4,92,0,2,1);//LCD12864_value(4,80,calendar.sec,1);
												break;
											default:break;											
									}
								}
								break;
							default:break;
					}
					break;
				case 3:
					
					break;
				case 5:		
					if(zoretmp>99.9)
					{	zoretmp=0;}
					else
					{	zoretmp+=0.1;}				
					LCDputvalue(zoretmp,5,106,1,5,0);
					if(up_down())
					{
						MUEN_ST++;
						r_tmp=1;
						s_tmp=0;
						LCD_DVmuen(DV_sel);
					}
					else
					{
						while(KEY1==0)
						{
							if(zoretmp>99.9)
							{	zoretmp=0;}
							else
							{	zoretmp+=0.1;}				
							LCDputvalue(zoretmp,5,106,1,5,0);//LCD12864_Pvalue(5,96,zoretmp);
							delay_ms(100);
						}		
					}						
					break;
				case 6:
					DV_sel=0;
					LCD_DVsel(DV_sel);
					break;
				case 7:
					D_sel=0;
					LCD_Dsel(D_sel);
					break;
				case 8:
					if(ktmp<2)
					{	ktmp+=0.001;}	
					LCDputvalue(ktmp,4,80,3,5,0);
					keytmp=0;
#if 1
					while(KEY1==0&&keytmp<20)
					{	
						delay_ms(50);
						keytmp++;
					}
					while(KEY1==0)
					{	
						delay_ms(20);
						if(ktmp<2)
						{	ktmp+=0.001;}	
						LCDputvalue(ktmp,4,80,3,5,0);
					}
#else
					while(KEY1==0&&keytmp<20)
					{	
						delay_ms(40);
						keytmp++;
					}
					while(KEY1==0&&keytmp<25)
					{	
						delay_ms(400);
						keytmp++;
						if(ktmp<2)
						{	ktmp+=0.001;}	
						LCDputvalue(ktmp,4,80,3,5,0);
					}
					while(KEY1==0&&keytmp<30)
					{	
						delay_ms(200);
						keytmp++;
						if(ktmp<2)
						{	ktmp+=0.001;}	
						LCDputvalue(ktmp,4,80,3,5,0);
					}
					while(KEY1==0&&keytmp<40)
					{	
						delay_ms(100);
						keytmp++;
						if(ktmp<2)
						{	ktmp+=0.001;}	
						LCDputvalue(ktmp,4,80,3,5,0);
					}
					while(KEY1==0)
					{	
						delay_ms(10);
						if(ktmp<2)
						{	ktmp+=0.001;}	
						LCDputvalue(ktmp,4,80,3,5,0);
					}
#endif
					break;
				default:break;
		}
		//EXTI_ClearITPendingBit(EXTI_Line13);  //清除LINE13上的中断标志位  
		while(KEY1==0)
		{
			while(KEY1==0)
			{
				delay_ms(50);
			}
			delay_ms(10);
		}
	}	
	else if(KEY2==0)	 //left按键
	{
		delay_ms(10);
		if(KEY2!=0)
		{	return;}
		switch(MUEN_ST)
		{
				case 0:
#if	ENGLISH
					english_flag=1;
#else
					english_flag=0;
					LCD12864_LAUG(0);
#endif
					if(left_right())
					{
						MUEN_ST=11;
						
					}
					break;
				case 1:	
					switch(ITEM_ST)
					{
							case 0:
								r_type=(r_type+6)%7;
								LCD12864_R_type(r_type,1);
								if((r_type!=6)&&(s_type!=2))
								{
									s_type=2;
									LCD12864_S_type(2,80,s_type,0);
								}
								break;
							case 1:
							#if 1
								if(r_type==6)
								{
									s_type=(s_type+(SPEED_SUM+SPEED_AUTO-1))%(SPEED_SUM+SPEED_AUTO);
								}
							#else
								s_type=(s_type+(SPEED_SUM+SPEED_AUTO-1))%(SPEED_SUM+SPEED_AUTO);
							#endif
#if	NDJ_4S|NDJ_7S
							if(s_type<S_BOTTOM)
							{	s_type=S_AT;}
							else if(s_type>S_TOP)
							{	s_type=S_TOP;}
#endif								
								LCD12864_S_type(2,80,s_type,1);
								if((r_type!=6)&&(s_type!=2))
								{
									r_type=6;
									LCD12864_R_type(r_type,0);
								}
								break;
							case 2:
								o_type=(o_type+1)%2;
								LCD12864_OUT_type(o_type,1);
								break;
							case 3:
								c_type=(c_type+1)%2;
								LCD12864_CLOCK_type(c_type,1);
								break;
							default:break;
					}				
					break;
				case 2:
					switch(ITEM_ST)
					{
							case 0:
								
								break;
							case 1:
								
								break;
							case 2:
								
								break;
							case 3:
								if(c_type)
								{
									if(c_sel)
									{
										LCD12864_uStime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,c_sel);
										c_sel--;
										LCD12864_Stime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,c_sel);
									}
								}
								break;
							default:break;
					}				
					break;
				case 3:
					
					break;
				case 5:
					s_tmp+=(SPEED_SUM-1);
					s_tmp%=SPEED_SUM;
#if	NDJ_4S|NDJ_7S
					if(s_tmp<S_BOTTOM)
					{	s_tmp=S_TOP;}
					else if(s_tmp>S_TOP)
					{	s_tmp=S_BOTTOM;}
#endif												
					zoretmp=Read_Target(Zero,s_tmp);
					zoretmp1=zoretmp;
					LCD12864_Ztype(s_tmp,zoretmp);
					break;
				case 8:
					if(r_tmp==6)
					{
						s_tmp=(s_tmp+(SPEED_SUM-1))%SPEED_SUM;
						if(s_tmp>=S_TOP)
						{
							r_tmp--;
							//s_tmp=2;
						}
					}
					else
					{
						r_tmp=(r_tmp+6)%7;
					}
					//s_tmp=(s_tmp+(SPEED_SUM-1))%SPEED_SUM;	
#if	NDJ_4S|NDJ_7S
					if(s_tmp<S_BOTTOM)
					{	s_tmp=S_TOP;}
					else if(s_tmp>S_TOP)
					{	s_tmp=S_BOTTOM;}					
#endif	
					//if(s_tmp>=S_TOP)
					//{
					//	s_tmp=S_TOP;
					//	r_tmp=(r_tmp+6)%7;
					//}
					ktmp=Read_Target((r_tmp+1),s_tmp);
					ktmp1=ktmp;
					LCD_VK(r_tmp,s_tmp,ktmp);
					break;
				default:break;
		}
		//EXTI_ClearITPendingBit(EXTI_Line14);  //清除LINE14上的中断标志位  
		while(KEY2==0)
		{
			while(KEY2==0)
			{
				delay_ms(50);
			}
			delay_ms(10);
		}
	}		 
	else if(KEY3==0)	 //back按键
	{
		delay_ms(10);
		if(KEY3!=0)
		{	return;}
		switch(MUEN_ST)
		{
				case 0:
					
					break;
				case 1:				
					MUEN_ST--;
					LCD12864_Clr();
					LCD12864_start();					
					break;
				case 2:
					switch(ITEM_ST)
					{
							case 0:
							case 1:
								if(motor_start==1)
								{
									motor_start=0;
									start_cnt=0;
									auto_meas=0;
									//TIM_Cmd(TIM3, DISABLE);
									trig_flag=0;
									StopMt(s_type);
									LCD12864_Halt();
								}
								else
								{
									MUEN_ST--;
									LCD12864_muenCH();
									lint_cnt=0;
								}							
								break;
							case 2:
								MUEN_ST--;
								LCD12864_muenCH();
								break;
							case 3:
								MUEN_ST--;
								rtc_set=0;
								c_sel=0;
								RTC_ITConfig(RTC_IT_SEC, DISABLE);
								LCD12864_muenCH();
								break;
							default:break;
					}				
					break;
				case 3:
					
					break;
				case 5:
					MUEN_ST=0;
					s_tmp=0;
					zoretmp=0;
					zoretmp1=0;
					LCD12864_Clr();
					LCD12864_start();
					break;
				case 6:
					MUEN_ST--;
					zoretmp=Read_Target(Zero,s_type);
					zoretmp1=zoretmp;
					LCD12864_Zname();
					LCD12864_Ztype(s_type,zoretmp);
					break;
				case 8:
						MUEN_ST=6;
						LCD_DVmuen(DV_sel);						
						break;
				case	10:
						step_autoZ++;
						step_autoZ%=SPEED_SUM;
						s_type++;
						//P_Angle=0.1;
						if(s_type>=SPEED_SUM)
						{
								MUEN_ST=0;
								s_type=0;
								step_JDT=0;
							trig_flag=0;
								autoZ_flag=0;
								motor_start=0;								
								LCD12864_Clr();
								LCD12864_start();
								//TIM_Cmd(TIM3, DISABLE);	
								StopMt(s_type);
						}
						else
						{
							LCD12864_cJDT();
							step_JDT=0;
							trig_flag=0;
							LCD12864_Ztype(s_type,0);
						}
						break;
				default:break;	    
		}
		//EXTI_ClearITPendingBit(EXTI_Line15);  //清除LINE15上的中断标志位  
		while(KEY3==0)
		{
			while(KEY3==0)
			{
				delay_ms(50);
			}
			delay_ms(10);
		}
	}		
	else if(KEY4==0)	 	 //down按键
	{				 
		delay_ms(10);
		if(KEY4!=0)
		{	return;}
		switch(MUEN_ST)
		{
				case 0:				
					if(up_down())
					{
							MUEN_ST=5;
							r_tmp=1;
							s_tmp=0;
#if	NDJ_4S|NDJ_7S
							if(s_tmp<S_BOTTOM)
							{	s_tmp=S_BOTTOM;}
#endif							
							if(s_tmp>S_TOP)
							{	s_tmp=S_TOP;}
							zoretmp=Read_Target(Zero,s_tmp);
							zoretmp1=zoretmp;
							LCD12864_Zname();
							LCD12864_Ztype(s_tmp,zoretmp);
					}
					break;
				case 1:				
					ITEM_ST++;
					ITEM_ST=ITEM_ST%4;
					LCD12864_ClrItem(ITEM_ST,0);		

					if(up_down())
					{
							MUEN_ST=10;
							step_autoZ=0;
							autoZ_flag=1;								
							motor_start=1;
							motor_trig=1;
							step_JDT=0;//进度条进度
						trig_flag=0;	
							s_type=S_BOTTOM;
							zoretmp=0;
							ktmp=1;
							LCD12864_Zname();
							LCD12864_Ztype(s_type,zoretmp);
							THB6128_Turn_On(s_type);
					}
					break;
				case 2:
					switch(ITEM_ST)
					{
							case 0:
								
								break;
							case 1:
								
								break;
							case 2:
								
								break;
							case 3:
								if(c_type)
								{
									rtc_set=1;
									switch(c_sel)
									{
											case 0:
												if(english_flag==0)
												{
													calendar.w_year--;
													if(calendar.w_year==2014)
													{	calendar.w_year=2015; }
													LCDputvalue((float)calendar.w_year,1,42,0,4,1);
												}
												else
												{
													calendar.w_date--;
													if(calendar.w_date==0)
													{
														switch(calendar.w_month)
														{
																case 1:
																	calendar.w_date=31;
																	break;
																case 2:	
																	if((calendar.w_year%4)||((calendar.w_year%100==0)&&(calendar.w_year%400)))
																	{
																		calendar.w_date=28;
																	}
																	else
																	{
																		calendar.w_date=29;
																	}
																	break;
																case 3:
																	calendar.w_date=31;
																	break;
																case 4:
																	calendar.w_date=30;
																	break;
																case 5:
																	calendar.w_date=31;
																	break;
																case 6:
																	calendar.w_date=30;
																	break;
																case 7:
																case 8:
																	calendar.w_date=31;
																	break;
																case 9:
																	calendar.w_date=30;
																	break;
																case 10:
																	calendar.w_date=31;
																	break;
																case 11:
																	calendar.w_date=30;
																	break;
																case 12:
																	calendar.w_date=31;
																	break;
																default:break;	
														}
													}
													LCDputvalue((float)calendar.w_date,1,42,0,2,1);
												}
												break;
											case 1:	
												calendar.w_month--;
												if(calendar.w_month==0)
												{	calendar.w_month=12;	}
												if(english_flag==0)
												{
													LCDputvalue((float)calendar.w_month,1,70,0,2,1);
												}
												else
												{
													LCDputvalue((float)calendar.w_month,1,64,0,2,1);
												}
												break;
											case 2:
												if(english_flag==0)
												{
													calendar.w_date--;
													if(calendar.w_date==0)
													{
														switch(calendar.w_month)
														{
																case 1:
																	calendar.w_date=31;
																	break;
																case 2:	
																	if((calendar.w_year%4)||((calendar.w_year%100==0)&&(calendar.w_year%400)))
																	{
																		calendar.w_date=28;
																	}
																	else
																	{
																		calendar.w_date=29;
																	}
																	break;
																case 3:
																	calendar.w_date=31;
																	break;
																case 4:
																	calendar.w_date=30;
																	break;
																case 5:
																	calendar.w_date=31;
																	break;
																case 6:
																	calendar.w_date=30;
																	break;
																case 7:
																case 8:
																	calendar.w_date=31;
																	break;
																case 9:
																	calendar.w_date=30;
																	break;
																case 10:
																	calendar.w_date=31;
																	break;
																case 11:
																	calendar.w_date=30;
																	break;
																case 12:
																	calendar.w_date=31;
																	break;
																default:break;	
														}
													}
													LCDputvalue((float)calendar.w_date,1,98,0,2,1);
												}
												else
												{
													calendar.w_year--;
													if(calendar.w_year==2014)
													{	calendar.w_year=2015; }
													LCDputvalue((float)calendar.w_year,1,99,0,4,1);
												}
												break;
											case 3:
												calendar.hour--;
												if(calendar.hour==0)
												{	calendar.hour=23; }
												LCDputvalue((float)calendar.hour,4,49,0,2,1);//LCD12864_value(4,32,calendar.hour,1);
												break;
											case 4:
												calendar.min--;
												if(calendar.min==0)
												{	calendar.min=59; }
												LCDputvalue((float)calendar.min,4,71,0,2,1);//LCD12864_value(4,56,calendar.min,1);
												break;
											case 5:
												calendar.sec--;
												if(calendar.sec==0)
												{	calendar.sec=59; }
												LCDputvalue((float)calendar.sec,4,92,0,2,1);//LCD12864_value(4,80,calendar.sec,1);
												break;
											default:break;											
									}
								}
								break;
							default:break;
					}
					break;
				case 3:
					
					break;
				case 5:
					if(zoretmp>0.1)
					{	zoretmp-=0.1;}
					else
					{	zoretmp=0;}					
					LCD12864_Ztype(s_tmp,zoretmp);
					if(up_down())
					{
						MUEN_ST++;
						r_tmp=1;
						s_tmp=0;
						LCD_DVmuen(DV_sel);
					}
					else
					{
						while(KEY4==0)
						{
							if(zoretmp>0.1)
							{	zoretmp-=0.1;}
							else
							{	zoretmp=0;}					
							LCD12864_Ztype(s_tmp,zoretmp);
							delay_ms(100);
						}		
					}
					break;
				case 6:
					DV_sel=1;
					LCD_DVsel(DV_sel);
					break;
				case 7:
					D_sel=1;
					LCD_Dsel(D_sel);
					break;
				case 8:
					if(ktmp>0.501)
					{	ktmp-=0.001;}
					LCDputvalue(ktmp,4,80,3,5,0);
					keytmp=0;
#if 1
					while(KEY4==0&&keytmp<20)
					{	
						delay_ms(50);
						keytmp++;
					}
					while(KEY4==0)
					{	
						delay_ms(20);
						if(ktmp>0.501)
						{	ktmp-=0.001;}
						LCDputvalue(ktmp,4,80,3,5,0);
					}
#else
					while(KEY4==0&&keytmp<20)
					{	
						delay_ms(40);
						keytmp++;
					}
					while(KEY4==0&&keytmp<25)
					{	
						delay_ms(400);
						keytmp++;
						if(ktmp>0.501)
						{	ktmp-=0.001;}
						LCDputvalue(ktmp,4,80,3,5,0);
					}
					while(KEY4==0&&keytmp<30)
					{	
						delay_ms(200);
						keytmp++;
						if(ktmp>0.501)
						{	ktmp-=0.001;}
						LCDputvalue(ktmp,4,80,3,5,0);
					}
					while(KEY4==0&&keytmp<40)
					{	
						delay_ms(100);
						keytmp++;
						if(ktmp>0.501)
						{	ktmp-=0.001;}
						LCDputvalue(ktmp,4,80,3,5,0);
					}
					while(KEY4==0)
					{	
						delay_ms(10);
						if(ktmp>0.501)
						{	ktmp-=0.001;}
						LCDputvalue(ktmp,4,80,3,5,0);
					}
#endif
					break;			
				default:break;
		}	
		while(KEY4==0)
		{
			while(KEY4==0)
			{
				delay_ms(50);
			}
			delay_ms(10);
		}
	}
	else if(KEY5==0)	  //right按键
	{
		delay_ms(10);
		if(KEY5!=0)
		{	return;}
		switch(MUEN_ST)
		{
				case 0:
#if	ENGLISH
					english_flag=1;
#else
					english_flag=1;
					LCD12864_LAUG(1);
#endif
					break;
				case 1:	
					switch(ITEM_ST)
					{
							case 0:
								r_type++;
								r_type=r_type%7;
								LCD12864_R_type(r_type,1);
								if((r_type!=6)&&(s_type!=2))
								{
									s_type=2;
									LCD12864_S_type(2,80,s_type,0);
								}
								break;
							case 1:
							#if 1
								if(r_type==6)
								{
									s_type++;
								}
							#else
								s_type++;
							#endif
								s_type=s_type%(SPEED_SUM+SPEED_AUTO);
#if	NDJ_4S|NDJ_7S
							if(s_type<S_BOTTOM)
							{	s_type=S_BOTTOM;}
							else if(s_type>S_TOP)
							{	s_type=S_AT;}
#endif																											
								LCD12864_S_type(2,80,s_type,1);
								if((r_type!=6)&&(s_type!=2))
								{
									r_type=6;
									LCD12864_R_type(r_type,0);
								}
								break;
							case 2:
								o_type++;
								o_type=o_type%2;
								LCD12864_OUT_type(o_type,1);
								break;
							case 3:
								c_type++;
								c_type=c_type%2;
								LCD12864_CLOCK_type(c_type,1);
								break;
							default:break;
					}				
					break;
				case 2:
					switch(ITEM_ST)
					{
							case 0:
								
								break;
							case 1:
								
								break;
							case 2:
								
								break;
							case 3:
								if(c_type)
								{
									if(c_sel<5)
									{
										LCD12864_uStime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,c_sel);
										c_sel++;
										LCD12864_Stime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,c_sel);
									}
								}
								break;
							default:break;
					}
					break;
				case 3:
					
					break;
				case 5:
					s_tmp++;
					s_tmp%=SPEED_SUM;
#if	NDJ_4S|NDJ_7S
					if(s_tmp<S_BOTTOM)
					{	s_tmp=S_TOP;}
					else if(s_tmp>S_TOP)
					{	s_tmp=S_BOTTOM;}
#endif													
					zoretmp=Read_Target(Zero,s_tmp);
					zoretmp1=zoretmp;
					LCD12864_Ztype(s_tmp,zoretmp);
					break;
				case 8:
					if(r_tmp==6)
					{
						s_tmp++;
						if(s_tmp>S_TOP)
						{
							r_tmp=0;
							s_tmp=2;
						}
					}
					else 
					{
						r_tmp=(r_tmp+1)%7;
						if(r_tmp==6)
						{
							s_tmp=0;
						}
					}
// 					s_tmp++;									
// 					if(s_tmp>S_TOP)
// 					{
// 						r_tmp=(r_tmp+1)%7;
// 						s_tmp=0;
// 					}
#if	NDJ_4S|NDJ_7S
					if(s_tmp<S_BOTTOM)
					{	s_tmp=S_TOP;}
					else if(s_tmp>S_TOP)
					{	s_tmp=S_BOTTOM;}

#endif																		
					ktmp=Read_Target((r_tmp+1),s_tmp);
					ktmp1=ktmp;
					LCD_VK(r_tmp,s_tmp,ktmp);
					break;
				default:break;
		}
		while(KEY5==0)
		{
			while(KEY5==0)
			{
				delay_ms(50);
			}
			delay_ms(10);
		}
	}	
//////////////////////////////////////////////////////////////////////////////////////////////////////	
	else if(KEY6==0)	 	 //enter按键
	{				 
		delay_ms(10);
		if(KEY6==0)
		{
			switch(MUEN_ST)
			{
					case 0:
						MUEN_ST++;
#if	NDJ_4S|NDJ_7S
					if(s_type<S_BOTTOM)
					{	s_type=S_BOTTOM;}
					else if(s_type>S_TOP)
					{	s_type=S_AT;}
#endif													
						LCD12864_muenCH();
						break;
					case 1:				
						switch(ITEM_ST)
						{
								case 0:
								case 1:
									MUEN_ST++;								
									motor_start=1;
									motor_trig=1;
									step_JDT=0;//进度条进度
								trig_flag=0;	
									P_Angle=0.1;		
								
									if(s_type==S_AT)
									{
										auto_meas=1;
										s_type=S_BOTTOM;
										LCDputstring("AT",2,2,35,1);
									}
									zoretmp=Read_Target(Zero,s_type);
									ktmp=Read_Target((r_type+1),s_type);
									V_Range=VisRg[r_type][s_type];

									LCD12864_R_type(r_type,0);
									LCD12864_S_type(2,80,s_type,0);
									LCD12864_Meas(0);
									THB6128_Turn_On(s_type);
									break;
								case 2:
									LCD12864_Clr();
									if(o_type)
									{
										if(english_flag==1)
										{
											LCDputstring("PRINTING...",11,3,0,0);
										}
										else
										{
											LCDputstring("正在打印...",11,3,0,0);											
										}
										RTC_Get();
										PRINT_module(calendar,r_type,s_type,temper,Vis,Angle);
										LCD12864_Clr();
										LCD12864_muenCH();
									}
									else
									{
										if(english_flag==1)
										{
											LCDputstring("SENDING...",10,3,0,0);
										}
										else
										{
											LCDputstring("正在发送...",11,3,0,0);										
										}									
										LCD12864_Clr();
										LCD12864_muenCH();
									}	
									break;
								case 3:
									MUEN_ST++;
									
									if(c_type)
									{
										RTC_Get();
										LCD12864_Ptime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,1);
									}
									else
									{
										RTC_ITConfig(RTC_IT_SEC, ENABLE);
										LCD12864_Ptime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,0);
									}
									break;
								default:break;
						}									
						break;
					case 2:
						switch(ITEM_ST)
						{
								case 0:
								case 1:
									if(motor_start==0)
									{
										motor_start=1;
										motor_trig=1;
										step_JDT=0;//进度条进度
										trig_flag=0;
										P_Angle=0.1;
										LCD12864_Resume(s_type);
										THB6128_Turn_On(s_type);
									}
									break;
								case 2:
									break;
								case 3:
									if(c_type)
									{
										if(rtc_set==1)
										{
											RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
											LCD12864_Ptime(calendar.hour,calendar.min,calendar.sec,calendar.w_year,calendar.w_month,calendar.w_date,0);
											c_type=0;
											c_sel=0;
											RTC_ITConfig(RTC_IT_SEC, ENABLE);
										}
										rtc_set=0;	
									}
									break;
								default:break;
						}
						break;
					case 3:
						
						break;
					case 5:
						if(zoretmp1!=zoretmp)
						{	
							Save_Target(zoretmp,Zero,s_tmp);
							//xieru eeprom//////////////////////////////////////
						}						
						s_tmp++;
						s_tmp%=SPEED_SUM;
#if	NDJ_4S|NDJ_7S
					if(s_tmp<S_BOTTOM)
					{	s_tmp=S_BOTTOM;}
					else if(s_tmp>S_TOP)
					{	s_tmp=S_TOP;}
#endif													
						zoretmp=Read_Target(Zero,s_tmp);
						zoretmp1=zoretmp;
						LCD12864_Ztype(s_tmp,zoretmp);
						break;
					case 6:
						if(DV_sel)
						{	
							MUEN_ST=8;
							r_tmp=6;
							s_tmp=2;
							ktmp=Read_Target((r_tmp+1),s_tmp);
							ktmp1=ktmp;
							LCD_Kmuen(r_tmp,s_tmp,ktmp);
						}
						else
						{	
							MUEN_ST++;
							LCD_Dmod(D_sel);
						}
						break;
					case 7:
						MUEN_ST=6;
						LCD_DVmuen(DV_sel);		
						if(D_sel)
						{
							few_flag=0x55;						
						}
						else
						{
							few_flag=0xAA;
						}
						AT24CXX_WriteOneByte(Adress_FEW0,few_flag);
						break;
					case 8:
						if(ktmp1!=ktmp)
						{	
							Save_Target(ktmp,(r_tmp+1),s_tmp);
						}	
						s_tmp++;
						if(s_tmp>=SPEED_SUM)
						{
							s_tmp=0;	
							r_tmp=(r_tmp+1)%5;
						}
#if		NDJ_4S|NDJ_7S
						if(s_tmp<S_BOTTOM)
						{	s_tmp=S_BOTTOM;}
#endif
						ktmp=Read_Target((r_tmp+1),s_tmp);
						LCD_VK(r_tmp,s_tmp,ktmp);
						ktmp1=ktmp;
						break;
					case	10:
						Save_Target(Angle,Zero,s_type);
						step_autoZ++;
						s_type++;
						s_type%=SPEED_SUM;
#if	NDJ_4S|NDJ_7S
					if(s_type<S_BOTTOM)
					{	s_type=S_BOTTOM;}
					else if(s_type>S_TOP)
					{	s_type=S_TOP;}
#endif													
						LCD12864_cJDT();
						step_JDT=0;
						trig_flag=0;
						P_Angle=0.1;
						LCD12864_Ztype(s_type,0);
						break;
					default:break;	    
			}
			while(KEY6==0)
			{
				while(KEY6==0)
				{
					delay_ms(50);
				}
				delay_ms(10);
			}
		}
	}
}

void display_JDT(void)
{
		unsigned int tmp_tim;
		static u8 angle_cnt;
		static u8 meas_cnt;
		if((motor_start==0)||(trig_flag==0))
		{	return;}
		
		if(step_JDT==0)
		{
			step_JDT++;
			LCD12864_JDT(step_JDT-1);
		}
		tmp_tim=TIM3_Get_Value()-lint_cnt;
		if((LINT0==0)&&(LINT1==0))
		{		
				if(tmp_tim>10000)
				{
							LCD12864_Meas(0);
				}
		}
		
		if((tmp_tim>sum_cnt[s_type])&&(angle_cnt!=4))
		{	angle_cnt=4;}
		else if((tmp_tim<sum_cnt[s_type])&&(tmp_tim>(3*sum_cnt[s_type]>>2))&&(angle_cnt!=3))
		{	angle_cnt=3;}
		else if((tmp_tim<(3*sum_cnt[s_type]>>2))&&(tmp_tim>(sum_cnt[s_type]>>1))&&(angle_cnt!=2))
		{	angle_cnt=2;}
		else if((tmp_tim<(sum_cnt[s_type]>>1))&&(tmp_tim>(sum_cnt[s_type]>>2))&&(angle_cnt!=1))
		{	angle_cnt=1;}
		else
		{	return;}	
		
		if(step_JDT<8)
		{		
			meas_cnt++;
			if(step_JDT==0)
			{	LCD12864_cJDT();}
			switch(s_type)
			{
				case S_7_5:
					if(meas_cnt>=4)
					{
						meas_cnt=0;
						step_JDT++;
					}
					break;
				case S_75:
					if(meas_cnt>=16)
					{
						meas_cnt=0;
						step_JDT++;
					}
					break;
				case S_750:
					if(meas_cnt>=100)
					{
						meas_cnt=0;
						step_JDT++;
					}
					break;
				default:break;
			}
			LCD12864_JDT(step_JDT-1);
		}
}

void meas_DP(void)
{
		static float Last_Angle=0; 
		u8 tmpxiao;
		volatile int tmpangle;

		static u8 meas_over_cnt;
		static u8 neg_pos_flag;
	
		single_flag=0;
		if((meas_flag!=1)||(motor_start!=1))
		{	
			//TIM_Cmd(TIM3, DISABLE);
			StopMt(s_type);
			trig_flag=0;
			return;
		}
		/********************************************************/
		//用于测试clint_cnt值在750转时出现异常，异常值在116000左右！
		//printf("%d\n",clint_cnt);
		/********************************************************/
		Y_Angle=(float)((float)(clint_cnt)/(sum_cnt[s_type]));
		//delay_us(1);
		Y_Angle=(Y_Angle*100)-zoretmp;
		//delay_us(1);
		Y_Angle*=ktmp;
		//delay_us(1);
		if(Y_Angle<0)
		{
				Y_Angle=0;
		}
		else if(Y_Angle>100)
		{
				Y_Angle=99.9;
		}
#if 0		
		Angle=Y_Angle;
#else
		//if((s_type<3)||(step_JDT<3))
		//if((s_type<S_75)||(step_JDT<4))
		if(step_JDT<4)
		{
			Angle=Y_Angle;
		}
		else
		{
			if(Y_Angle>Angle)
			{
				if(neg_pos_flag)
				{
					if(Y_Angle-Angle>autoAdjust)	
					{	meas_over_cnt++;}
					else
					{	meas_over_cnt=0;}
				}
				else
				{	
					neg_pos_flag=1;
					meas_over_cnt=0;
				}
			}
			else
			{
				if(neg_pos_flag==0)
				{
					if(Angle-Y_Angle>autoAdjust)	
					{	meas_over_cnt++;}
					else
					{	meas_over_cnt=0;}
				}
				else
				{	
					neg_pos_flag=0;
					meas_over_cnt=0;
				}
			}
			if(meas_over_cnt>5)
			{
					P_Angle=0.1;
			}
			
			P_Angle=P_Angle+0.000001;
			KG_Angle=P_Angle/(P_Angle+0.01);
			Angle=Angle+KG_Angle*(Y_Angle-Angle);
			P_Angle=(1-KG_Angle)*P_Angle;			
		}
#endif	
		
#if 1
		tmpangle=(int)(10*(Angle+0.05));
		//delay_us(1);
		Angle=(float)tmpangle/10;
#endif
		if(autoZ_flag==1)
		{
				LCD12864_Ztype(s_type,Angle);
				if(step_JDT>=8)
				{
						Save_Target(Angle,Zero,s_type);
						step_autoZ++;
						s_type++;
						LCD12864_cJDT();
						step_JDT=0;
						trig_flag=0;
						P_Angle=0.1;
						
						//if(step_autoZ==9)
						if(s_type>=SPEED_SUM)
						{
								MUEN_ST=0;
								s_type=0;
								autoZ_flag=0;
								motor_start=0;	
								trig_flag=0;
								//TIM_Cmd(TIM3, DISABLE);
								StopMt(s_type);
								LCD12864_Clr();
								LCD12864_start();
						}
						else
						{
							LCD12864_Ztype(s_type,Angle);	
						}
				}
		}
		else
		{			
// 				if(s_type==S_7_5)
// 				{
// 					if((Last_Angle-Angle<-0.5)||(Last_Angle-Angle>0.5))
// 					{
// 						Last_Angle=Angle;
// 					}
// 				}else
				{
					Last_Angle=Angle;
				}
				
				Vis=(float)(Last_Angle/100)*V_Range;
				if(few_flag==0xAA)
				{
					if(((int)(Vis+0.5))==((int)Vis))
					{
							Vis=(int)Vis;
					}
				}

				//delay_us(1);
				if(Vis<50)
				{	tmpxiao=2;}
				else	if(Vis<500)
				{	tmpxiao=1;}
				else
				{	tmpxiao=0;}				
				if(Last_Angle>=99)
				{
					LCDputstring("  OVER ",7,4,56,0);//LCDputvalue(Vis,4,84,tmpxiao,7,0);
					LCDputstring("OVER ",5,6,70,0);
				}
				else
				{
					LCDputvalue(Vis,4,105,tmpxiao,7,0);//LCD12864_Pvalue(4,72,Vis);
					LCDputvalue(Last_Angle,6,105,1,5,0);////////////////////////////////////////
				}
			
				if(auto_meas==1&&step_JDT>=8)
				{
					if(Angle>=90)
					{				
						P_Angle=0.1;
						if(s_tmp<=S_BOTTOM)
						{	/*请更换小号转子!*/
								MUEN_ST--;
								auto_meas=0;
								motor_start=0;
								start_cnt=0;
							  trig_flag=0;
								//TIM_Cmd(TIM3, DISABLE);
								StopMt(s_type);
								if(r_type<4)
								{r_type++;}
								s_type=S_AT;
								LCD12864_muenCH();
						}
						else
						{	
								s_type--;
								zoretmp=Read_Target(Zero,s_type);
								V_Range=VisRg[r_type][s_type];
								LCD12864_S_type(2,80,s_type,0);
								step_JDT=0;
								trig_flag=0;
								LCD12864_cJDT();
						}		
					}
					else if(Angle<=10)
					{
						P_Angle=0.1;
						if(s_type>=S_TOP)
						{	/*请更换大号转子!*/
								MUEN_ST--;
								auto_meas=0;
								motor_start=0;
								start_cnt=0;
								trig_flag=0;
								//TIM_Cmd(TIM3, DISABLE);
								StopMt(s_type);
								if(r_type>0)
								{	r_type--;}
								s_type=S_AT;
								LCD12864_muenCH();
						}
						else
						{	
							s_type++;
							zoretmp=Read_Target(Zero,s_type);
							V_Range=VisRg[r_type][s_type];
							LCD12864_S_type(2,80,s_type,0);
							step_JDT=0;
							trig_flag=0;
							LCD12864_cJDT();
						}		
					}
				}

////////////////////////////////////////////////////////////////////////////////////				
// 				temperature=(float)DS18B20_Get_Temp();	
// 				temperature/=10;
// 				LCDputvalue(temperature,0,105,1,5,0);
// 				LCDputstring("℃",1,0,106,0);
////////////////////////////////////////////////////////////////////////////////////
				//LCDputvalue(Angle,6,105,1,5,0);//LCD12864_Pvalue(6,96,(Angle*100));
		}
}

u8 up_down(void)
{
		u8 backstage=0;
		u8 return_cnt=0;
		u8 both_flag=0;
		while((KEY1==0)||(KEY4==0))
		{
			if((KEY1==0)&&(KEY4==0))
			{	
				delay_ms(50);
				if((KEY1==0)&&(KEY4==0))
				{
					if(backstage<50)
					{	backstage++;}
					else
					{break;}
				}
				else
				{	break;}									
			}
			else
			{
					delay_ms(50);
					if((KEY1!=0)||(KEY4!=0))
					{return_cnt++;}
					if(return_cnt>=10)
					{break;}
			}
		}
		if(backstage>=50)
		{
				both_flag=1;
		}
		return both_flag;
}

u8 left_right(void)
{
		u8 backstage=0;
		u8 return_cnt=0;
		u8 both_flag=0;
		while((KEY2==0)||(KEY5==0))
		{
			if((KEY2==0)&&(KEY5==0))
			{	
				delay_ms(50);
				if((KEY2==0)&&(KEY5==0))
				{
					if(backstage<50)
					{	backstage++;}
					else
					{break;}
				}
				else
				{	break;}									
			}
			else
			{
					delay_ms(50);
					if((KEY2!=0)||(KEY5!=0))
					{return_cnt++;}
					if(return_cnt>=10)
					{break;}
			}
		}
		if(backstage>=50)
		{
				both_flag=1;
		}
		return both_flag;
}

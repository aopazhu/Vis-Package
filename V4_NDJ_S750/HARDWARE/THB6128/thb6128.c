#include "thb6128.h"
#include "timer.h"
void StopMt(unsigned char tmp_s)
{	
	if(THB6128_EN_S)
	{
		TIM_Cmd(TIM4, DISABLE);
		THB6128_Slow_Stop(tmp_s);
		THB6128_EN(0);
	}
}

void THB6128_Config_Para(u16 arr,u16 psc,u8 mtmp)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_SetCompare2(TIM4,arr>>1);

	THB6128_M1(mtmp&0x1);
	THB6128_M2(mtmp&0x2);                                               
	THB6128_M3(mtmp&0x4);
	
	//TIM_Cmd(TIM4, ENABLE);  
}

void THB6128_Turn_On(unsigned char tmp_s)
{
	//THB6128_EN(0);
	//TIM_Cmd(TIM4, DISABLE); 	
	THB6128_Slow_Start(tmp_s);
	switch(tmp_s)
	{
		case S_7_5:
			THB6128_Config_Para(899,24,7);
			break;
		case S_75:
			THB6128_Config_Para(449,9,6);
			break;
		case S_750:
			THB6128_Config_Para(179,4,5);
			break;
		default:return;
	}
	TIM_Cmd(TIM4, ENABLE);  		
	THB6128_EN(1);
}

void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器4时钟
 	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟 
 
   //设置该引脚为复用输出功能,输出TIM4 CH2的PWM脉冲波形	GPIOB.7
	GPIO_InitStructure.GPIO_Pin = THB6128_CLK_Pin;//TIM4_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC2

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
 
	TIM_SetCompare2(TIM4,arr>>1);
 
	TIM_Cmd(TIM4, DISABLE);  
}

void THB6128_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	//定义结构体		
	
	RCC_APB2PeriphClockCmd(THB6128_GPCLK|RCC_APB2Periph_AFIO, ENABLE);  //使能功能复用IO时钟，不开启复用时钟不能显示
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);    //把调试设置普通IO口

	GPIO_InitStructure.GPIO_Pin  = THB6128_M1_Pin;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //开漏输出//数据口配置成开漏输出模式，此模式下读输入寄存器的值得到IO口状态
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(THB6128_M1_GP , &GPIO_InitStructure);    //IO口初始化函数（使能上述配置）
	
	GPIO_InitStructure.GPIO_Pin  = THB6128_M2_Pin;	
	GPIO_Init(THB6128_M2_GP , &GPIO_InitStructure);    //IO口初始化函数（使能上述配置）

	GPIO_InitStructure.GPIO_Pin  = THB6128_M3_Pin;	
	GPIO_Init(THB6128_M3_GP , &GPIO_InitStructure);    //IO口初始化函数（使能上述配置）

	GPIO_InitStructure.GPIO_Pin  = THB6128_EN_Pin;	
	GPIO_Init(THB6128_EN_GP , &GPIO_InitStructure);    //IO口初始化函数（使能上述配置）

	GPIO_InitStructure.GPIO_Pin  = THB6128_CW_Pin;	
	GPIO_Init(THB6128_CW_GP , &GPIO_InitStructure);    //IO口初始化函数（使能上述配置）

	THB6128_EN(0);
	
	TIM4_PWM_Init(899,124);

	THB6128_M1(0);
	THB6128_M2(0);
	THB6128_M3(1);
	
	THB6128_CW(1);
	
	//TIM_Cmd(TIM4, ENABLE);
}
#define		Tc	1
void THB6128_Slow_Start(unsigned char tmp_s)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned int tmp_sK;
	unsigned int tmp_t,tmp_t0,tmp_t1,tmp_t2,tmp_t3;

	tmp_t0=tmp_t2=TIM3_Get_Value();
	tmp_t1=tmp_t3=0;
	
	switch(tmp_s)
	{
		case S_75:
			tmp_sK=Tc*62500000;//Tc*1000000/(Vs/60*12800)*1000000
			break;
		case S_750:
			tmp_sK=Tc*6250000;//Tc*1000000/(Vs/60*12800)*1000000
			break;
		default:return;
	}
	
	GPIO_InitStructure.GPIO_Pin = THB6128_CLK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//初始化GPIO
	
	THB6128_M1(1);
	THB6128_M2(0);
	THB6128_M3(1);
	THB6128_EN(1);
	
	while(tmp_t1<(Tc*1000000))
	{
		tmp_t=TIM3_Get_Value();
		tmp_t1=tmp_t-tmp_t0;
		tmp_t3=tmp_t-tmp_t2;
		if((tmp_sK/tmp_t1)<=tmp_t3)
		{
			THB6128_CLK;
			tmp_t2=tmp_t;
		}
	}	
	
	THB6128_EN(0);
	
	GPIO_InitStructure.GPIO_Pin = THB6128_CLK_Pin;//TIM4_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//初始化GPIO
	
}

void THB6128_Slow_Stop(unsigned char tmp_s)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned int tmp_sK;
	unsigned int tmp_t,tmp_t0,tmp_t1,tmp_t2,tmp_t3,tmp_t4;

	tmp_t0=tmp_t2=TIM3_Get_Value();
	tmp_t1=tmp_t3=0;
	
	switch(tmp_s)
	{
		case S_75:
			tmp_sK=Tc*62500000;//Tc*1000000/(Vs/60*12800)*1000000
			break;
		case S_750:
			tmp_sK=Tc*6250000;//Tc*1000000/(Vs/60*12800)*1000000
			break;
		default:return;
	}
	
	GPIO_InitStructure.GPIO_Pin = THB6128_CLK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//初始化GPIO
	
	THB6128_M1(1);
	THB6128_M2(0);
	THB6128_M3(1);
	//THB6128_EN(1);
	
	while(tmp_t1<(Tc*1000000))
	{
		tmp_t=TIM3_Get_Value();
		tmp_t1=tmp_t-tmp_t0;
		tmp_t4=(Tc*1000000)-tmp_t1;
		tmp_t3=tmp_t-tmp_t2;
		if((tmp_sK/tmp_t4)<=tmp_t3)
		{
			THB6128_CLK;
			tmp_t2=tmp_t;
		}
	}	
	
// 	THB6128_EN(0);
	
	GPIO_InitStructure.GPIO_Pin = THB6128_CLK_Pin;//TIM4_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//初始化GPIO
	
}





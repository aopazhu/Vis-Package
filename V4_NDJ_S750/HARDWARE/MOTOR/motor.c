#include "stm32f10x.h"
#include"motor.h"
#include"delay.h"	
extern double	K_slow0;
extern double	K_slow;
float	T_slow=10;

unsigned int	T_sum=(64000/3);

static	unsigned int sum_cnt;
static	unsigned int single_cnt=0;
	
unsigned char xf_step;                                           //细分步数（二相电机单二拍走一循环为64步） 
unsigned char xf32_sin[64]={0,6,12,18,24,31,37,43,               //32细分后走一个循环的细分数据；
                                  48,54,60,65,71,76,81,85,
                                  90,94,98,102,106,109,112,115,
                                  118,120,122,124,125,126,127,127,
                                  127,127,127,126,125,124,122,120,
                                  118,115,112,109,106,102,98,94,
                                  90,85,81,76,71,65,60,54,
                                  48,43,37,31,24,18,12,6
                                 }; 
unsigned char xf32_cos[64]={127,127,127,126,125,124,122,120,
                                  118,115,112,109,106,102,98,94,
                                  90,85,81,76,71,65,60,54,
                                  48,43,37,31,24,18,12,6,
                                  0,6,12,18,24,31,37,43,
                                  48,54,60,65,71,76,81,85,
                                  90,94,98,102,106,109,112,115,
                                  118,120,122,124,125,126,127,127,
                                 };

void MOTOR_InitPort(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	//定义结构体		
	
	RCC_APB2PeriphClockCmd(MOTOR_CLK|RCC_APB2Periph_AFIO, ENABLE);  //使能功能复用IO时钟，不开启复用时钟不能显示
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);    //把调试设置普通IO口

	GPIO_InitStructure.GPIO_Pin  = M_Data;		//数据口配置成开漏输出模式，此模式下读输入寄存器的值得到IO口状态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(Motor_D , &GPIO_InitStructure);    //IO口初始化函数（使能上述配置）

	GPIO_InitStructure.GPIO_Pin  = M_WR;		//数据口配置成开漏输出模式，此模式下读输入寄存器的值得到IO口状态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(Motor_WR , &GPIO_InitStructure);     //IO口初始化函数（使能上述配置）
	
	GPIO_InitStructure.GPIO_Pin  = M_ACS;		//数据口配置成开漏输出模式，此模式下读输入寄存器的值得到IO口状态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(Motor_ACS , &GPIO_InitStructure);     //IO口初始化函数（使能上述配置）
	
	GPIO_InitStructure.GPIO_Pin  = M_RST;		//数据口配置成开漏输出模式，此模式下读输入寄存器的值得到IO口状态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(Motor_RST , &GPIO_InitStructure);     //IO口初始化函数（使能上述配置）

	Mot_Data(0);
	Mot_WR(1);
	Mot_A0(0);
	Mot_A1(0);
	Mot_CS(1);
	Mot_RST(1);
	delay_ms(10);
	Mot_RST(0);

	K_slow0=T_sum*200*T_slow;
}
																 
void StopMt(void) 
{    
	Mot_A0(0);                         //选DA通道0；
	Mot_Data(0);
	Mot_WR(0);                         //DA数据写入；
	delay_us(1); 
	Mot_WR(1);
	Mot_A0(1);                         //选DA通道1；
	Mot_Data(0);
	Mot_WR(0);                         //DA数据写入；
	delay_us(1);	
	Mot_WR(1);	

	Mot_RST(1);
	delay_ms(10);
	Mot_RST(0);
	
	sum_cnt=0;
	single_cnt=0;
}

void MOTOR_updata(void)
{
		
	static u8 xifen=0,n=0;
	
	Mot_A0(0);
	Mot_A1(0); 
	Mot_CS(0);                        //选DA通道0；	
	if((n>32)&(n<96))
	{	Mot_Data((xf32_cos[xifen]&0x7F));}
	else
	{	Mot_Data((xf32_cos[xifen]|0x80));}
	Mot_WR(0);                         //DA数据写入；
	//-delay_us(10); 
	Mot_WR(1);
	//-delay_us(2); 
	Mot_CS(1);
	//-delay_us(5); 
	
	Mot_A0(1);
	Mot_A1(0);
	Mot_CS(0);                        //选DA通道0；	
	if((n>=64)&(n<=128))
	{	Mot_Data((xf32_sin[xifen]|0x80));}
	else
	{	Mot_Data((xf32_sin[xifen]&0x7F));}
	Mot_WR(0);                         //DA数据写入；
	//-delay_us(10); 
	Mot_WR(1);
	//-delay_us(2); 
	Mot_CS(1);
	//-delay_us(5); 	

	n++;
	if(n>=128)
		n=0;
	xifen++;
	if(xifen>=64)
		xifen=0;
//	xf_step++;                           //0.5ms进次中断，电机走一步，每走64步重新循环
//	xf_step=xf_step%64;
}


u8 Motor_SlowStart(void)
{
// 	static	unsigned int sum_cnt;
// 	static	unsigned int single_cnt=0;
	double	K_tmp;

	sum_cnt++;
	K_tmp=K_slow/sum_cnt;
	if(K_tmp<=(double)single_cnt)
	{
		single_cnt=0;
		
		MOTOR_updata();
		
		if(sum_cnt>((unsigned int)(T_sum*T_slow)))
		{			
				sum_cnt=0;
				return 1;
		}
	}
	else
	{
		single_cnt++;
	}	
	return 0;
}

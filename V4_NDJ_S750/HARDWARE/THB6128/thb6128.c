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
	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��4ʱ��
 	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ�� 
 
   //���ø�����Ϊ�����������,���TIM4 CH2��PWM���岨��	GPIOB.7
	GPIO_InitStructure.GPIO_Pin = THB6128_CLK_Pin;//TIM4_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM4 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC2

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���
 
	TIM_SetCompare2(TIM4,arr>>1);
 
	TIM_Cmd(TIM4, DISABLE);  
}

void THB6128_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	//����ṹ��		
	
	RCC_APB2PeriphClockCmd(THB6128_GPCLK|RCC_APB2Periph_AFIO, ENABLE);  //ʹ�ܹ��ܸ���IOʱ�ӣ�����������ʱ�Ӳ�����ʾ
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);    //�ѵ���������ͨIO��

	GPIO_InitStructure.GPIO_Pin  = THB6128_M1_Pin;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //��©���//���ݿ����óɿ�©���ģʽ����ģʽ�¶�����Ĵ�����ֵ�õ�IO��״̬
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(THB6128_M1_GP , &GPIO_InitStructure);    //IO�ڳ�ʼ��������ʹ���������ã�
	
	GPIO_InitStructure.GPIO_Pin  = THB6128_M2_Pin;	
	GPIO_Init(THB6128_M2_GP , &GPIO_InitStructure);    //IO�ڳ�ʼ��������ʹ���������ã�

	GPIO_InitStructure.GPIO_Pin  = THB6128_M3_Pin;	
	GPIO_Init(THB6128_M3_GP , &GPIO_InitStructure);    //IO�ڳ�ʼ��������ʹ���������ã�

	GPIO_InitStructure.GPIO_Pin  = THB6128_EN_Pin;	
	GPIO_Init(THB6128_EN_GP , &GPIO_InitStructure);    //IO�ڳ�ʼ��������ʹ���������ã�

	GPIO_InitStructure.GPIO_Pin  = THB6128_CW_Pin;	
	GPIO_Init(THB6128_CW_GP , &GPIO_InitStructure);    //IO�ڳ�ʼ��������ʹ���������ã�

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
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//��ʼ��GPIO
	
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//��ʼ��GPIO
	
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
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//��ʼ��GPIO
	
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(THB6128_CLK_GP, &GPIO_InitStructure);//��ʼ��GPIO
	
}





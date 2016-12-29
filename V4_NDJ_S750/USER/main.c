#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "12864.h"
#include "rtc.h"
#include "timer.h"
#include "thb6128.h"
#include "adc.h"
#include "at24cxx.h"
#include "i2c.h"
#include "print.h"
#include "pt100.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"



u8 english_flag=0;
u8 meas_flag=0;
u8 few_flag=0;

u8 r_type=6;
u8 s_type=2;
u8 o_type=0;
u8 c_type=0;
u8 MUEN_ST=0;
u8 ITEM_ST=0;

u8 rtc_set=0;
//u8 s_cnt=0;
u8 motor_start=0;
u8 motor_trig=0;
//u8 backstage=0;
u8 ds18b20_flag=1;
uint16_t start_cnt=0;
unsigned int lint_cnt=0;

float Angle=0,Vis=0;

float temper;

float	Vref=3300;
float	Rref=160;
float	Ktemp=1;//0.9865;

const float Rx = 3240.0;

float	V_slow;

double	K_slow0;
double	K_slow;

typedef	struct
{
		float	OUT;
		float	IN;
		float P;
		float	KG;
		float	Dth;
		unsigned char		Dcnt;
		unsigned char		Dcth;
		
}Kalman,*Kalman_ptr;

void	fun_kalman(Kalman_ptr kalm_p)
{
		float	Dtmp=kalm_p->OUT-kalm_p->IN;
		
		if(Dtmp<0)
		{
				Dtmp=-Dtmp;
		}
		if(Dtmp>kalm_p->Dth)
		{
				kalm_p->Dcnt++;
		}
		else
		{
				kalm_p->Dcnt=0;
		}
		
		if(kalm_p->Dcnt>kalm_p->Dcth)
		{
				kalm_p->P=1;
				kalm_p->Dcnt=0;
		}
		kalm_p->P=kalm_p->P+0.00000001;
		kalm_p->KG=kalm_p->P/(kalm_p->P+0.01);
		kalm_p->OUT=kalm_p->OUT+kalm_p->KG*(kalm_p->IN-kalm_p->OUT);
		kalm_p->P=(1-kalm_p->KG)*kalm_p->P;	
}

void	printloop(void);
#if 0
int main(void)
{
	u16 adc_value;
	float value;
	Kalman klm_value={0,0,2,0,100,0,100};
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Adc_Init();	 
	
	while(1)
	{

		adc_value=Get_Adc(ADC_Channel_8);	
		//value=(float)adc_value/4096;
		klm_value.IN=(float)adc_value;
		fun_kalman(&klm_value);
			
		value=(float)klm_value.OUT/4096;
		value=value/5;
		value=value+0.032258;
		value=1/value;
		value=3000/(value-1);
// 		value=value-0.645161;
// 		value=1/value;
// 		value=value-0.05;
// 		value=1/value;
// 		value=1.5*value;
		
		delay_ms(20);		
		
	}
}
#else
int main(void)
 {		
	u8 rtc_error_flag=0;
	u16 adc_value;
	u8	tmprom;  
	unsigned int tmp_cnt=0;
	Kalman	tmpkalm={	0,0,2,0,5,0,10};
	
	delay_init();	    	 //延时函数初始化	  
		
	LCD12864_InitPort(); //端口初始化
	LCD12864_Init();	 //液晶初始化	
	LCD12864_Clr(); 
	LCD12864_start();   

	AT24CXX_Init(); 
	tmprom=AT24CXX_Check(); 	 
	if(tmprom)
	{
			Cover_eeprom();
	}	 
	
	few_flag=(u8)AT24CXX_ReadLenByte((Adress_FEW0),1);
	
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为115200
	
	EXTIX_Init();
	
	THB6128_Init();
	//MOTOR_InitPort();
	//StopMt(); 
	TIM3_Int_Init(0xffff,71);
	Adc_Init();	 
	rtc_error_flag=RTC_Init();
	
#if	ENGLISH
	tmp_cnt=30;
	while(tmp_cnt--)
	{
		english_flag=1;
		KEY_Scan();
		if(MUEN_ST>0)
		{	break;}
		delay_ms(100);		
	}
	MUEN_ST=1;
		#if	NDJ_4S
		if(s_type<S_5)
		{	s_type=S_5;}
		else if(s_type>S_50)
		{	s_type=S_50;}
		#elif	NDJ_7S
		if(s_type<S_0_5)
		{	s_type=S_0_5;}
		else if(s_type>S_50)
		{	s_type=S_50;}
		#endif						
		LCD12864_muenCH();	
#endif
 	while(1)
	{
		if(rtc_error_flag==1)
		{	
				rtc_error_flag=RTC_Init();
		}
		KEY_Scan();
		display_JDT();
		if(meas_flag==1)
		{
				meas_DP();
				meas_flag=0;	
				//printf("%7.2f\n",Angle);
				//printloop();
		}
		else
		{
				tmp_cnt++;
				if(tmp_cnt>=5000)
				{
					tmp_cnt=0;
					adc_value=Get_Adc(ADC_Channel_8);		
					if(adc_value>3000)
					{
						temper=-1;
					}
					else
					{
						tmpkalm.IN=(float)adc_value;
						fun_kalman(&tmpkalm);
						temper=tmpkalm.OUT;
						#if 0
						temper=temper/4096;
						temper=(temper*Vref/Rref)*Ktemp;
						temper=temper-4;
						temper=temper*400/16;
						#else
						temper=temper/4096;
						temper=temper/10;
						//temper=temper+0.032258;//1/31=0.032258
						//temper=temper+0.030303;//1/33=0.030303
						//temper=temper+0.02994;//1/33.4=0.02994
						temper=temper+(100/(Rx+100));
						temper=1/temper;
						temper=Rx/(temper-1);
						temper=ResToTmp(temper);
						#endif
					}
					if((MUEN_ST==1)||(motor_start))
					{						
						if(temper<0)
						{	LCDputstring("       ",7,0,70,0);}
						else
						{
							LCDputvalue(temper,0,105,1,5,0);
							LCDputstring("℃",1,0,106,0);	
						}												
					}
				}
		}
		
	}	 
 }
#endif

void	printloop(void)
{
		if(o_type)
		{	return;}
		printf("%d#",r_type);
		switch(s_type)
		{
				case 0:
					printf("0.1RPM");
					break;
				case 1:
					printf("0.2RPM");
					break;
				case 2:
					printf("0.5RPM");
					break;
				case 3:
					printf(" 1 RPM");
					break;
				case 4:
					printf(" 2 RPM");
					break;
				case 5:
					printf(" 5 RPM");
					break;
				case 6:
					printf("10 RPM");
					break;
				case 7:
					printf("20 RPM");
					break;
				case 8:
					printf("50 RPM");
					break;
				case 9:
					printf("100RPM");
					break;
				case 10:
					printf("200RPM");
					break;
				default:break;
		}
		if(Vis<10)
		{
			printf("%7.2f",Vis);
		}
		else if(Vis<1000)
		{
			printf("%7.1f",Vis);
		}
		else
		{
			printf("%7.0f",Vis);	
		}
		printf("%5.1f",Angle);
		if(temper>0&&temper<100)/////////////待添加提示或默认
			printf("%5.1f",temper);		
		else
			printf("     ");				
}
 


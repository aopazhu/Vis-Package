#ifndef __MOTOR_H_
#define __MOTOR_H_


#define  	MOTOR_CLK          RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD

#define		Motor_D    	GPIOA
#define		M_Data	GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7

#define		Motor_WR    GPIOB
#define		M_WR		GPIO_Pin_3

#define		Motor_ACS   GPIOC
#define		M_ACS		GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12

#define		M_A0		GPIO_Pin_10
#define		M_A1		GPIO_Pin_11
#define		M_CS		GPIO_Pin_12

#define		Motor_RST   GPIOD
#define		M_RST		GPIO_Pin_2

#define   Mot_WR(x)   x ? GPIO_SetBits(Motor_WR,M_WR):   GPIO_ResetBits(Motor_WR,M_WR)
#define   Mot_A0(x)   x ? GPIO_SetBits(Motor_ACS,M_A0):  GPIO_ResetBits(Motor_ACS,M_A0)
#define   Mot_A1(x)   x ? GPIO_SetBits(Motor_ACS,M_A1):  GPIO_ResetBits(Motor_ACS,M_A1)
#define   Mot_CS(x)   x ? GPIO_SetBits(Motor_ACS,M_CS):  GPIO_ResetBits(Motor_ACS,M_CS)
#define   Mot_RST(x)  x ? GPIO_SetBits(Motor_RST,M_RST): GPIO_ResetBits(Motor_RST,M_RST)

#define  	Mot_Data(x)   {Motor_D->BSRR = (x&0x00ff);Motor_D->BRR=((~x)&0x00ff);}

void MOTOR_InitPort(void);
void StopMt(void);
void MOTOR_updata(void);
u8 Motor_SlowStart(void);
void cal_K_slow(void);

#endif

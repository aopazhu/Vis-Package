#ifndef __THB6128_H
#define __THB6128_H
#include "sys.h"
#include "stm32f10x_tim.h"

#define  	THB6128_GPCLK       (RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)

#define		THB6128_M1_GP				GPIOB
#define		THB6128_M1_Pin			GPIO_Pin_3
#define		THB6128_M2_GP				GPIOD
#define		THB6128_M2_Pin			GPIO_Pin_2
#define		THB6128_M3_GP				GPIOC
#define		THB6128_M3_Pin			GPIO_Pin_12

#define		THB6128_EN_GP				GPIOC
#define		THB6128_EN_Pin			GPIO_Pin_11
#define		THB6128_CW_GP				GPIOC
#define		THB6128_CW_Pin			GPIO_Pin_10

#define		THB6128_CLK_GP			GPIOB
#define		THB6128_CLK_Pin			GPIO_Pin_7


#define   THB6128_M1(x)   x ? GPIO_SetBits(THB6128_M1_GP,THB6128_M1_Pin):   GPIO_ResetBits(THB6128_M1_GP,THB6128_M1_Pin)
#define   THB6128_M2(x)   x ? GPIO_SetBits(THB6128_M2_GP,THB6128_M2_Pin):   GPIO_ResetBits(THB6128_M2_GP,THB6128_M2_Pin)
#define   THB6128_M3(x)   x ? GPIO_SetBits(THB6128_M3_GP,THB6128_M3_Pin):   GPIO_ResetBits(THB6128_M3_GP,THB6128_M3_Pin)

#define   THB6128_EN(x)   x ? GPIO_SetBits(THB6128_EN_GP,THB6128_EN_Pin):   GPIO_ResetBits(THB6128_EN_GP,THB6128_EN_Pin)
#define   THB6128_CW(x)   x ? GPIO_SetBits(THB6128_CW_GP,THB6128_CW_Pin):   GPIO_ResetBits(THB6128_CW_GP,THB6128_CW_Pin)
#define		THB6128_CLK			(THB6128_CLK_GP->ODR ^= GPIO_Pin_7)

#define  	THB6128_EN_S    GPIO_ReadInputDataBit(THB6128_EN_GP,THB6128_EN_Pin)

void StopMt(unsigned char tmp_s);
void THB6128_Turn_On(unsigned char tmp_s);
void THB6128_Init(void);
void THB6128_Slow_Start(unsigned char tmp_s);
void THB6128_Slow_Stop(unsigned char tmp_s);
#endif
